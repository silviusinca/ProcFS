#include <aio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#define NO_PROCESSES 100

int main()
{
    char *nume[3] = {"test0", "test1", "test2"};
    printf("Me %d Parent %d\n", getpid(), getppid());
    for (int i = 0; i < 3; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
            return errno;
        else if (pid == 0)
        {
            mkdir(nume[i], S_IRWXU);
            exit(1);
        }
    }
    for (int i = 0; i < 3; i++)
        wait(NULL);
    printf("Hai sa vedem ");
    exit(0);
}