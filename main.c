#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

struct processInfo
{
    char **process;
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
    result.process = (char **)malloc(count * sizeof(char *));

    while ((pDirent = readdir(procDir)) != NULL)
    {
        if (isdigit(pDirent->d_name[0]))
            result.process[i++] = pDirent->d_name;
    }
    closedir(procDir);
    return result;
}

int main()
{
    while (1)
    {
        mkdir("procFS", S_IRWXU);
        struct processInfo processes = get_processes();
        for (int i = 0; i < processes.size; i++)
        {
            char aux[25], statsFile[30], procFile[25], ch;
            sprintf(procFile, "/proc/%s/status", processes.process[i]);
            sprintf(aux, "procFS/%s", processes.process[i]);
            mkdir(aux, S_IRWXU);
            sprintf(statsFile, "%s/stats", aux);

            int statsFD = open(statsFile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
            if (statsFD == NULL)
            {
                printf("Cannot open stats from process %s", processes.process[i]);
                return errno;
            }

            // dup2(statsFD, STDOUT_FILENO);
            // close(statsFD);
            // if (pid == 0)
            // {
            //     char *argv[] = {"/bin/ps", "-Flww", "-p", test.processes[i], NULL};
            //     execve("/bin/ps", argv, NULL);
            // }
            // else
            //     wait(NULL);

            pid_t pid = fork();
            if (pid < 0)
                return errno;
            else if (pid == 0)
            {
                char *argv[] = {"/bin/cp", procFile, statsFile, NULL};
                execve("/bin/cp", argv, NULL);
            }
            else
                wait(NULL);
        }

        pid_t pid = fork();
        if (pid < 0)
            return errno;
        else if (pid == 0)
        {
            sleep(3);
            char *argv[] = {"/bin/rm", "-rf", "procFS", NULL};
            execve("/bin/rm", argv, NULL);
        }
        else
            wait(NULL);
    }
    return 0;
}
