#define SLEEP_TIME 100 * 1000
#define CHILD_COUNT 2
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

pid_t childs[CHILD_COUNT];
int msgCount = 0;

int timeMSec()
{
    struct timeval time;
    int res = gettimeofday(&time, NULL);
    if (res == -1){
        perror("Cannot get current time: ");
        return -1;
    };

    int msec = time.tv_sec * 1000 + time.tv_usec / 1000;

    return msec;
}

void parentCallback(int sig, siginfo_t *siginfo, void *context) {
    printf("%3d PID: %6d PPID: %6d Time: %ld PARENT gets %s from %d\n", msgCount, getpid(), getppid(), timeMSec(),strsignal(sig), siginfo->si_pid);

    usleep(SLEEP_TIME);


    if (kill(0, SIGUSR1) == -1) {
        perror("Can not send any signal to any process\n");
        exit(-1);
    }else{
        printf("%3d PID: %6d PPID: %6d Time: %ld PARENT puts User defined signal 1\n", msgCount++, getpid(), getppid(), timeMSec());
    }
}

void childCallback(int sig, siginfo_t *siginfo, void *context) {
    pid_t pid = getpid();
    pid_t ppid = getppid();

    int childNumber = pid == childs[0] ? 1 : 2;

    printf("%3d PID: %6d PPID: %6d Time: %ld CHILD%d gets %s\n", msgCount, pid, ppid, timeMSec(), childNumber,strsignal(sig));

    if (kill(ppid, SIGUSR2) == -1) {
        perror("Can not send any signal to any process\n");
        exit(-1);
    }else
    {
        printf("%3d PID: %6d PPID: %6d Time: %ld CHILD%d puts User defined signal 2\n", msgCount++, pid, ppid, timeMSec(), childNumber);
    }
}

void main(void){
    sigset_t mask1, mask2;

    if (sigemptyset(&mask1) == -1) {
        perror("Can not clear mask1: ");
        exit(-1);
    }

    if (sigaddset(&mask1, SIGUSR1) == -1) {
        perror("Can not add SIGUSR1 to mask1: ");
        exit(-1);
    }

    if (sigemptyset(&mask2) == -1) {
        perror("Can not clear mask2: ");
        exit(-1);
    }

    if (sigaddset(&mask2, SIGUSR2) == -1) {
        perror("Can not add SIGUSR2 to mask2: ");
        exit(-1);
    }

    struct sigaction parentSigact, childSigact;
    memset(&parentSigact, 0, sizeof(parentSigact));
    parentSigact.sa_sigaction = parentCallback;
    parentSigact.sa_flags = SA_SIGINFO;

    memset(&childSigact, 0, sizeof(childSigact));
    childSigact.sa_sigaction = childCallback;
    childSigact.sa_flags = SA_SIGINFO;

    for (int i = 0; i < CHILD_COUNT; i++) {
        childs[i] = fork();

        switch (childs[i]) {
            case 0:
                childs[i] = getpid();
                if (sigprocmask(SIG_SETMASK, &mask2, 0) == -1) {
                    perror("Can not change the signal mask for Child\n");
                    exit(-1);
                }
                if (sigaction(SIGUSR1, &childSigact, NULL) == -1) {
                    perror("Can not change the action for Child\n");
                    exit(-1);
                }
                printf("PID: %d PPID: %d Time: %ld CHILD%d\n", getpid(), getppid(), timeMSec(), i + 1);
                while (1) {}

            case -1:
                perror("Can not create a child process\n")	;
        }
    }

    printf("PID: %d PPID: %d Time: %ld PARENT\n", getpid(), getppid(), timeMSec());

    if (sigprocmask(SIG_SETMASK, &mask1, 0) == -1) {
        perror("Can not change the signal mask for Parent\n");
        exit(-1);
    }

    if (sigaction(SIGUSR2, &parentSigact, NULL) == -1) {
        perror("Can not change the action for Parent\n");
        exit(-1);
    }

    sleep(1);

    if (kill(0, SIGUSR1) == -1) {
        perror("Can not send any signal to any process\n");
        exit(-1);
    }
    else{
        printf("%3d PID: %6d PPID: %6d Time: %ld PARENT puts User defined signal 1\n", msgCount, getpid(), getppid(), timeMSec());
    }

    while (1) {}

    return;
}
