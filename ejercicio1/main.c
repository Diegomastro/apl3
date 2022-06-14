#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void printStatus(int processNumber) {
    printf("Proceso %d - PID %d - Padre %d\n", processNumber, getpid(), getppid());
}

int main(int argc, char const* argv[]) {
    // somos el proceso 1
    pid_t firsProcessPid = getpid();
    printStatus(1);
    pid_t secondProcess = fork();

    if (!secondProcess) {
        // child process
        printStatus(2);
        pid_t fifthProcess = fork();

        if (!fifthProcess) {
            printStatus(5);

            pid_t twelveProcess = fork();

            if (!twelveProcess) {
                printStatus(12);
            }
        } else {
            pid_t sixthProcess = fork();

            if (!sixthProcess) {
                printStatus(6);

                pid_t ninthProcess = fork();

                if (!ninthProcess) {
                    printStatus(9);
                } else {
                    pid_t tenthProcess = fork();

                    if (!tenthProcess) {
                        printStatus(10);
                    }
                }
            }
        }
    } else {
        pid_t thirdProcess = fork();
        if (!thirdProcess) {
            pid_t seventhProcess = fork();

            if (!seventhProcess) {
                printStatus(7);
            } else {
                printStatus(3);
            }
        } else {
            pid_t fourthProcess = fork();

            if (!fourthProcess) {
                pid_t eightProcess = fork();
                if (!eightProcess) {
                    // process 8 is init's child 
                    kill(getppid(), SIGTERM);
                    printStatus(8);
                    pid_t processEleven = fork();

                    if (!processEleven) {
                        printStatus(11);
                    }
                }
            }
        }
    }
    sleep(10);
    return 0;
}
