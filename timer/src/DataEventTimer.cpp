#include "DataEventTimer.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <set>
#include <list>
#include <time.h>
#include <mutex>
#include <semaphore.h>
#include <sys/prctl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "osal_log.h"

#ifdef TEST
#define LOGD(_fmt, _args...)  do {fprintf(stdout, "[D][TIME] " _fmt " [%s][%d]\n",##_args, __FILE__,__LINE__);fflush(stdout);} while(0);
#define LOGW(_fmt, _args...)  do {fprintf(stdout, "[W][TIME] " _fmt " [%s][%d]\n",##_args, __FILE__,__LINE__);fflush(stdout);} while(0);
#define LOGE(_fmt, _args...)  do {fprintf(stderr, "[E][TIME] " _fmt " [%s][%d]\n",##_args, __FILE__,__LINE__);fflush(stderr);} while(0);
#endif

inline bool CmpTimespec(
        const timespec& a,
        const timespec& b)
{
    if (a.tv_sec < b.tv_sec)
    {
        return true;
    }
    else if (a.tv_sec == b.tv_sec && a.tv_nsec < b.tv_nsec)
    {
        return true;
    }
    else
    {
        return false;
    }
}

class TimerTask
{
public:

    timespec stopTime;
    uint64_t cycleTime;
    TimerTaskCallback cb;
    uint32_t taskId;
    int64_t times;

    TimerTask(
            timespec _stopTime,
            uint64_t _cycleTime,
            TimerTaskCallback _cb,
            uint32_t _taskId,
            int64_t _times) :
            stopTime(_stopTime), cycleTime(_cycleTime), cb(_cb), taskId(_taskId), times(_times)
    {
    }

    bool IsTimerout(
            const timespec& curTime) const
    {
        if (this->stopTime.tv_sec < curTime.tv_sec)
        {
            return true;
        }
        else if (this->stopTime.tv_sec == curTime.tv_sec && this->stopTime.tv_nsec <= curTime.tv_nsec)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator<(
            const TimerTask& other) const
    {
        return CmpTimespec(this->stopTime, other.stopTime);
    }
};

bool CmpTimerTaskPtr(
        TimerTask* const & left,
        TimerTask* const & right)
{
    return *left < *right;
}

struct ProcTimerTaskType
{
    enum TIMER_TASK_TYPE
    {
        TIMER_TASK_ADD = 0, TIMER_TASK_DEL = 1, TIMER_TASK_MAX
    };

    TIMER_TASK_TYPE type;
    void* data;

    ProcTimerTaskType(
            TIMER_TASK_TYPE _type,
            void* _data) :
            type(_type), data(_data)
    {
    }
};

static std::set< TimerTask*, std::function< bool(
        TimerTask* const &,
        TimerTask* const &) > > s_setTimerTask(CmpTimerTaskPtr);

static std::list< ProcTimerTaskType* > s_listProcTimerTask;
static std::mutex s_mutexListProcTimerTask;  //任务set锁

static uint32_t s_u32IdSeed = 0;

static pthread_t s_pPrcoTimerTaskThread; //
static int s_fdSignalPipe[2] = { -1, -1 }; //

static void SendProcSignal()
{
    char buf = 0;
    write(s_fdSignalPipe[1], &buf, 1);
}

static void FormCurTimeUpdateTimerTask(
        TimerTask * task,
        const timespec& curTime)
{
    task->stopTime.tv_sec = curTime.tv_sec;
    task->stopTime.tv_nsec = curTime.tv_nsec;

    task->stopTime.tv_sec += task->cycleTime / 1000;
    if ((task->stopTime.tv_nsec + ((task->cycleTime % 1000) * 1000000)) >= 1000000000)
    {
        task->stopTime.tv_sec += 1;
        task->stopTime.tv_nsec -= 1000000000;
    }
    task->stopTime.tv_nsec += (task->cycleTime % 1000) * 1000000;
}

uint32_t GetTimerTaskId()
{
    static std::mutex s_mutexTimerTaskIdSeed;
    s_mutexTimerTaskIdSeed.lock();
    ++s_u32IdSeed;
    if (s_u32IdSeed == 0)
    {
        ++s_u32IdSeed;
    }
    s_mutexTimerTaskIdSeed.unlock();
    return s_u32IdSeed;
}

static void addTimerTask(
        TimerTask * task)
{
    s_mutexListProcTimerTask.lock();
    s_listProcTimerTask.push_back(new ProcTimerTaskType(ProcTimerTaskType::TIMER_TASK_ADD, task));
    s_mutexListProcTimerTask.unlock();

    SendProcSignal(); //发送处理信号
}

void addTimerTask(
        uint32_t taskId,
        uint64_t millisecond,
        TimerTaskCallback cb,
        int64_t times)
{
    if (millisecond == 0 || times == 0)
    {
        return;
    }

    timespec stopTime;
    clock_gettime(CLOCK_MONOTONIC, &stopTime);
    stopTime.tv_sec += millisecond / 1000;
    if ((stopTime.tv_nsec + ((millisecond % 1000) * 1000000)) >= 1000000000)
    {
        stopTime.tv_sec += 1;
        stopTime.tv_nsec -= 1000000000;
    }
    stopTime.tv_nsec += (millisecond % 1000) * 1000000;

    addTimerTask(new TimerTask(stopTime, millisecond, cb, taskId, times));
}

uint32_t addTimerTask(
        uint64_t millisecond,
        TimerTaskCallback cb,
        int64_t times)
{
    uint32_t curId = GetTimerTaskId();
    addTimerTask(curId, millisecond, cb, times);
    return curId;
}

void deleteTimerTask(
        uint32_t TimerTaskId)
{
    s_mutexListProcTimerTask.lock();
    s_listProcTimerTask.push_back(new ProcTimerTaskType(ProcTimerTaskType::TIMER_TASK_DEL, new decltype(TimerTaskId)(TimerTaskId)));
    s_mutexListProcTimerTask.unlock();

    LOGD("delete Timer Task id %u", TimerTaskId);
    SendProcSignal(); //发送处理信号
}

void alterTimerTask(
        uint32_t taskId,
        uint64_t millisecond,
        TimerTaskCallback cb,
        int64_t times)
{
    deleteTimerTask(taskId);
    addTimerTask(taskId, millisecond, cb, times);
}

static void* prcoTimerTask(
        void *)
{
    LOGD("start timer thread");
    prctl(PR_SET_NAME, "TimerProc");
    fd_set rfds;
    timeval waitTime = { 20, 0 }; //初始化为20秒
    while (true)
    {
        FD_ZERO(&rfds);
        FD_SET(s_fdSignalPipe[0], &rfds);

        int ret = select(s_fdSignalPipe[0] + 1, &rfds, NULL, NULL, &waitTime);
        if (ret < 0)
        {
            if (errno == EAGAIN)
            {
                LOGW("select return warning %d: %s", errno, strerror(errno));
                continue;
            }
            else
            {
                LOGE("select return error %d: %s", errno, strerror(errno));
                break;
            }
        }
        if (FD_ISSET(s_fdSignalPipe[0], &rfds))
        {
            char buf[128];
            int r = read(s_fdSignalPipe[0], buf, sizeof(buf));
            if (r <= 0)
            {
                LOGE("read pipe error %d: %s", errno, strerror(errno));
                break;
            }
        }

        decltype(s_listProcTimerTask) listProcTimerTaskTmp;

        s_mutexListProcTimerTask.lock();
        listProcTimerTaskTmp.swap(s_listProcTimerTask);
        s_mutexListProcTimerTask.unlock();

        for (auto it : listProcTimerTaskTmp)
        {
            if (it->type == ProcTimerTaskType::TIMER_TASK_ADD)
            {
                s_setTimerTask.insert((TimerTask*) (it->data));
            }
            else if (it->type == ProcTimerTaskType::TIMER_TASK_DEL)
            {
                uint32_t deleteTaskId = *((uint32_t*) (it->data));
                delete (uint32_t*) (it->data);

                for (auto it1 = s_setTimerTask.begin(); it1 != s_setTimerTask.end();)
                {
                    if ((*it1)->taskId == deleteTaskId)
                    {
                        delete (*it1);
                        s_setTimerTask.erase(it1++);
                    }
                    else
                    {
                        ++it1;
                    }
                }
            }
            else
            {
                // LOGE("unknow TimerTask");
            }
            delete it;
        }

        timespec curTime;
        clock_gettime(CLOCK_MONOTONIC, &curTime);
        //处理任务
        for (auto it = s_setTimerTask.begin(); it != s_setTimerTask.end();)
        {
            TimerTask* curTask = (*it);
            if (curTask->IsTimerout(curTime))
            {
                curTask->cb();
                if (curTask->times < 0) //循环定时器
                {
                    FormCurTimeUpdateTimerTask(curTask, curTime);
                    addTimerTask(curTask);
                }
                else if (curTask->times > 0 && curTask->times != 1) //剩下不止一次
                {
                    --(curTask->times);
                    FormCurTimeUpdateTimerTask(curTask, curTime);
                    addTimerTask(curTask);
                }
                else //剩下一次或零次
                {
                    delete curTask;
                }
                s_setTimerTask.erase(it++); //删除本次迭代器
            }
            else
            {
                break;
            }
        }

        //更新下一次延时时间
        if (s_setTimerTask.empty() == false)
        {
            auto it = s_setTimerTask.begin();
            int64_t diffSec = (*it)->stopTime.tv_sec - curTime.tv_sec;
            int32_t diffNsec = (*it)->stopTime.tv_nsec - curTime.tv_nsec;

            if (diffNsec < 0)
            {
                --diffSec;
                diffNsec += 1000000000;
            }
            waitTime.tv_sec = diffSec;
            waitTime.tv_usec = diffNsec / 1000000;
        }
        else
        {
            waitTime.tv_sec = 20;
            waitTime.tv_usec = 0;
        }
    }
    LOGE("stop timer thread");
    return nullptr;
}

static struct initTimer
{
    initTimer()
    {
        int ret = pipe(s_fdSignalPipe);
        if (ret < 0)
        {
            LOGE("create pipe error %d: %s", errno, strerror(errno));
            return;
        }

        ret = pthread_create(&s_pPrcoTimerTaskThread, nullptr, prcoTimerTask, nullptr);
        if (ret < 0)
        {
            LOGE("create thread error %d: %s", errno, strerror(errno));
            return;
        }

        LOGD("init timer success");
    }
} s_initTimer;
