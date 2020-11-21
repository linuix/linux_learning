#ifndef __DATAEVENTTIMER_H__
#define __DATAEVENTTIMER_H__

#include <functional>
#include <stdint.h>
typedef std::function< void(
        void) > TimerTaskCallback;

/**
 * 获取定时任务id
 * */
uint32_t GetTimerTaskId();

/**
 * 添加定时任务
 * taskId：任务id 从GetTimerTaskId中获取
 * millisecond：延时执行时间，0为不执行
 * cb：执行函数
 * times：执行次数，<0为循环任务，>0按次数执行，0为不执行
 * */
void addTimerTask(
        uint32_t taskId,
        uint64_t millisecond,
        TimerTaskCallback cb,
        int64_t times);

/**
 * 添加定时任务
 * millisecond：延时执行时间，0为不执行
 * cb：执行函数
 * times：执行次数，<0为循环任务，>0按次数执行，0为不执行
 * return：定时任务id
 * */
uint32_t addTimerTask(
        uint64_t millisecond,
        TimerTaskCallback cb,
        int64_t times);

/**
 * 删除定时任务
 * TimerTaskId：定时任务的id
 * */
void deleteTimerTask(
        uint32_t TimerTaskId);

/**
 * 修改定时任务
 * taskId：任务id，从GetTimerTaskId或addTimerTask中获取
 * millisecond：延时执行时间，0为不执行
 * cb：执行函数
 * times：执行次数，<0为循环任务，>0按次数执行，0为不执行
 * */
void alterTimerTask(
        uint32_t taskId,
        uint64_t millisecond,
        TimerTaskCallback cb,
        int64_t times);

#endif
