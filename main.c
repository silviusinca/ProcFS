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
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>

struct processInfo
{
    char **processes;
    size_t size;
};

struct processInfo get_processes()
{
    size_t count = 0;
    struct dirent *pDirent;
    DIR *procDir = opendir("/proc/");

    if (procDir == NULL)
    {
        printf("Cannot open directory /proc/\n");
        return;
    }
    while ((pDirent = readdir(procDir)) != NULL)
    {
        if (isdigit(pDirent->d_name[0]))
            count++;
    }
    size_t i = 0;
    closedir(procDir);
    procDir = opendir("/proc/");
    struct processInfo result;
    result.size = count;
    result.processes = (char **)malloc(count * sizeof(char *));

    while ((pDirent = readdir(procDir)) != NULL)
    {
        if (isdigit(pDirent->d_name[0]))
            result.processes[i++] = pDirent->d_name;
    }
    closedir(procDir);
    return result;
}

int main()
{
    char aux[12];
    while (1)
    {
        mkdir("procFS", S_IRWXU);
        struct processInfo test = get_processes();
        for (int i = 0; i < test.size; i++)
        {
            sprintf(aux, "procFS/%s", test.processes[i]);
            mkdir(aux, S_IRWXU);
        }
        pid_t pid = fork();
        if (pid < 0)
            return errno;
        else if (pid == 0)
        {
            sleep(1);
            char *argv[] = {"/bin/rm", "-rf", "procFS", NULL};
            execve("/bin/rm", argv, NULL);
        }
        else
            wait(NULL);
    }
    return 0;
}
