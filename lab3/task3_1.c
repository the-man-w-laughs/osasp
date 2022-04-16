#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


void printInfo(const char* whoPrints)
{
    struct timeval time;
    gettimeofday(&time, NULL);

    int hr = (time.tv_sec / 3600 + 3) % 24; // because of Belarus (GMT +3 hours)
    int min = (time.tv_sec / 60) % 60;
    int sec = time.tv_sec % 60;
    int msec = time.tv_usec / 1000;


    printf("%-12s: ppid = %d; pid = %d; time:%02d:%02d:%02d:%03d.\n", whoPrints, getppid(), getpid(), hr, min, sec, msec);

return;	
}

int main()
{
    int firstPid = fork();
    switch (firstPid)
    {
        case -1:
            // error
            perror("first fork");
            return -1;
        case 0:
            // first child
            printInfo("First child");
            return 1;
        default:
            {
                int secondPid = fork();
                switch (secondPid)
                {
                    case -1:
                        // error
                        perror("second fork");
                        return -1;
                    case 0:
                        // second child
                        printInfo("Second child");
                        return 1;
                    default:
                        {
                            // Parent
                            printInfo("Parent");
                            system("ps -x");
                            int status;
                            waitpid(firstPid, &status, 0);
                            printf("First exit normally? %s\n", (WIFEXITED(status) ? "true" : "false"));
                            waitpid(secondPid, &status, 0);
                            printf("Second exit normally? %s\n", (WIFEXITED(status) ? "true" : "false"));
                            break;
                        }
                }
            }
    }
    return 0;
}
