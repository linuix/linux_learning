#include <time.h>
#include <unistd.h>
#include <log_util.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

void process_signal(int signal)
{
    LOGD("signal = %d", signal);
}

int main(int argc, char const *argv[])
{

    time_t t;
    time(&t);
    LOGD("%ld", t);
    signal(SIGALRM, process_signal);
    alarm(3);
    // pause();
    sleep(10);
    LOGD("%ld", t);

    return 0;
}
