#define failure(str) {perror(str); exit(-1);}
#define CHILDS_COUNT 8
#define SIG_COUNT 101
#define FILE_NAME "tmpPID"
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//child item
typedef struct _child_t {
    int _index;
    int *_childs;
    int _childs_count;
    struct sigaction _act;
} child_t;

//all childs
child_t childs[CHILDS_COUNT + 1];

//counters for USR signals
int usr1Count = 0;

//signal action for SIGTERM
struct sigaction terminateSigact;

//initialization of a file with PIDs
void initFile(void) {
    FILE* fp = fopen(FILE_NAME, "w+b");
    if (fp == NULL)
    failure("Can not open temp file");
    int initial = 0;
    for (int i = 0; i <= CHILDS_COUNT; i++)
        fwrite(&initial, sizeof(int), 1, fp);

    if (fclose(fp) == EOF)
    failure("Can not close temp file");
    return;
}

//read all PIDs from a file
int* readAllPID(void) {
    FILE* fp = fopen(FILE_NAME, "rb");
    if (fp == NULL)
    failure("Can not open temp file\n");
    int* pids = (int*)malloc((CHILDS_COUNT + 1) * sizeof(int));
    if (pids == NULL)
    failure("Can not allocate memory");

    fread(pids, sizeof(int), CHILDS_COUNT + 1, fp);

    if (fclose(fp) == EOF)
    failure("Can not close temp file");

    return pids;
}

//write new PID to a file
void writePID(int ordChild, pid_t newPID) {

    int *pids = readAllPID();
    pids[ordChild] = newPID;

    FILE* fp = fopen(FILE_NAME, "w+b");
    if (fp == NULL)
    failure("Can not open temp file");
    fwrite(pids, sizeof(int), CHILDS_COUNT + 1, fp);
    if (fclose(fp) == EOF)
    failure("Can not close temp file");
    free(pids);
    return;
}

//read child PID from a file
pid_t readPID(int ordChild) {
    int *pids = readAllPID();
    pid_t pid = pids[ordChild];
    free(pids);
    return pid;
}

//get time in microseconds
long long getTime() {
    struct timeval tv;

    if (gettimeofday(&tv, NULL) == -1) {
        perror("Can not get current time");
        return -1;
    }

    return tv.tv_sec * 1000000 + tv.tv_usec;
}

//terminate work of the child process
void terminateCallback(int sig) {

    // who am I?
    int* pids = readAllPID();

    pid_t pid = getpid();

    int i = 0;
    while (i <= CHILDS_COUNT && pid != pids[i])
        i++;

    free(pids);
    // who am I?

    printf("%d PID: %d PPID: %d finished the work after %3ds SIGUSR1\n", i, pid, getppid(), usr1Count);

    for (int j = 0; j < childs[i]._childs_count; j++) {
        int kidsPid = readPID(childs[i]._childs[j]);
        kill(kidsPid, SIGTERM);
    }

    while (wait(0) > 0);

    exit(0);
}

void reseivedMessage(int who){
    printf("%d PID: %d PPID: %d received SIGUSR1 Time: %lld\n", who, getpid(), getppid(), getTime());
    usr1Count++;
}

//child 1 process signal
void child1Callback(int sig) {
    reseivedMessage(1);
    //wait all childs 2-8
    if (usr1Count == SIG_COUNT) {

        kill(-readPID(2), SIGTERM);

        int wpid;
        while (wait(0) > 0);

        printf("%d PID: %d PPID: %d finished the work after %3ds SIGUSR1\n",1, getpid(), getppid(),usr1Count);
        exit(0);
    }

    kill(-(readPID(2)), SIGUSR1);
    printf("%d PID: %d PPID: %d sent     SIGUSR1 Time: %lld\n", 1, getpid(), getppid(), getTime());

}

//child 2 process signal
void child2Callback(int sig) {
    reseivedMessage(2);
}

//child 3 process signal
void child3Callback(int sig) {
    reseivedMessage(3);
}

//child 4 process signal
void child4Callback(int sig) {
    reseivedMessage(4);
}

//child 5 process signal
void child5Callback(int sig) {
    reseivedMessage(5);
    if (kill(-(readPID(6)), SIGUSR1) == -1) failure("Can not sent     a signal\n");
    printf("%d PID: %d PPID: %d sent     SIGUSR1 Time: %lld\n", 5, getpid(), getppid(), getTime());

}

//child 6 process signal
void child6Callback(int sig) {
    reseivedMessage(6);
}

//child 7 process signal
void child7Callback(int sig) {
    reseivedMessage(7);
}

//child 8 process signal
void child8Callback(int sig) {
    reseivedMessage(8);
    if (kill(readPID(1), SIGUSR1) == -1)
    failure("Can not sent     a signal\n");
    printf("%d PID: %d PPID: %d sent     SIGUSR1 Time: %lld\n", 8, getpid(), getppid(), getTime());
}

//initialize child items
void formChildsTree(child_t *node) {
    for (int i = 0; i <= CHILDS_COUNT; i++) {
        memset(&node[i], 0, sizeof(node[i]));
        node[i]._index = i;
    }

    node[0]._childs_count = 1;
    node[0]._childs = (int*)malloc(sizeof(int) * node[0]._childs_count);
    if (node[0]._childs == NULL)
    failure("Can not allocate a memory");
    node[0]._childs[0] = 1;

    node[1]._childs_count = 4;
    node[1]._childs = (int*)malloc(sizeof(int) * node[1]._childs_count);
    if (node[1]._childs == NULL)
    failure("Can not allocate a memory");
    node[1]._childs[0] = 2;
    node[1]._childs[1] = 3;
    node[1]._childs[2] = 4;
    node[1]._childs[3] = 5;
    node[1]._act.sa_handler = child1Callback;

    node[2]._childs_count = 0;
    node[2]._act.sa_handler = child2Callback;

    node[3]._childs_count = 0;
    node[3]._act.sa_handler = child3Callback;

    node[4]._childs_count = 0;
    node[4]._act.sa_handler = child4Callback;

    node[5]._childs_count = 3;
    node[5]._childs = (int*)malloc(sizeof(int) * node[1]._childs_count);
    if (node[5]._childs == NULL)
    failure("Can not allocate a memory");
    node[5]._childs[0] = 6;
    node[5]._childs[1] = 7;
    node[5]._childs[2] = 8;
    node[5]._act.sa_handler = child5Callback;

    node[6]._childs_count = 0;
    node[6]._act.sa_handler = child6Callback;

    node[7]._childs_count = 0;

    node[7]._act.sa_handler = child7Callback;

    node[8]._childs_count = 0;
    node[8]._act.sa_handler = child8Callback;
}

//create process tree
void createChildTree(child_t node) {
    if (node._index == 2){
        setpgid(0, getpid());
    }
    if (node._index>2 && node._index<6){
        setpgid(0, readPID(2));
    }

    if (node._index == 6){
        setpgid(0, getpid());
    }
    if (node._index>6){
        setpgid(0, readPID(6));
    }

    printf("PID: %d PPID: %d GPID: %d Time: %lld CHILD%d\n", getpid(), getppid(),getpgrp(), getTime(), node._index);

    //add action for user signal
    if (node._index != 0)
        if (sigaction(SIGUSR1, &node._act, 0) == -1)
        failure("Can not change the action for child\n");

    //add action for terminate signal
    if (node._index > 1)
        if (sigaction(SIGTERM, &terminateSigact, 0) == -1)
        failure("Can not change the action for child\n");

    //write created PID to a file
    writePID(node._index, getpid());

    //create all childs for this parent
    for (int i = 0; i < node._childs_count; i++) {

        //wait while previous pid created
        while (readPID(node._childs[i] - 1) == 0) {}

        pid_t child = fork();
        switch(child) {

            //child process
            case 0:
                createChildTree(childs[node._childs[i]]);
                while (1) {}
                break;

                //child create failure
            case -1:
            failure("Can not create a child\n");
                break;

                //parent
            default:

                //child 1 process
                if (node._index == 1 && i + 1 == node._childs_count) {

                    //wait all pids
                    for (int i = 0; i <= CHILDS_COUNT; i++) {
                        while (readPID(i) == 0) {}
                    }

                    //start signal exchange
                    if (kill(-readPID(2), SIGUSR1) == -1)
                    failure("Can not send a signal\n");
                    while (1) {}
                }
                break;
        }
    }
    return;
}

void main(void) {

    //initialize file with PIDs
    initFile();

    //initialize child items
    formChildsTree(childs);

    //create terminate action
    memset(&terminateSigact, 0, sizeof(terminateSigact));
    terminateSigact.sa_handler = terminateCallback;

    //create process tree
    createChildTree(childs[0]);

    //wait for first child
    while (readPID(1) == 0) {}
    if (waitpid(readPID(1), NULL, 0) == -1)
    failure("Can not wait PID\n");

    printf("%d PID: %d PPID: %d finished the work after %3ds SIGUSR1\n",0, getpid(), getppid(), usr1Count);

    return;
}