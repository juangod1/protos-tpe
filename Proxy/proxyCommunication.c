#include "include/proxyCommunication.h"
#include "../Shared/include/lib.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


/*
 *  Executes Parser Process.
 *  Returns PROXY_READ_PIPE and Parser's Pid
 */
int start_parser(char * cmd,char * msg, size_t size, int * pid_ret)
{
    int pipes[NUM_PIPES][2];

    if(pipe(pipes[PROXY_READ_PIPE])==-1)
    {
        perror("pipe");
        return ERROR;
    }
    if(pipe(pipes[PROXY_WRITE_PIPE])==-1)
    {
        perror("pipe");
        return ERROR;
    }

    if(write(PROXY_WRITE_FD,msg,size)==-1)
    {
        perror("write");
        return ERROR;
    }

    pid_t pid=fork();

    if (pid==0)
    {
        close(PROXY_READ_FD);
        close(PROXY_WRITE_FD);

        dup2(PARSER_READ_FD, STDIN_FILENO);
        dup2(PARSER_WRITE_FD, STDOUT_FILENO);

        close(PARSER_READ_FD);
        close(PARSER_WRITE_FD);

        char *argv[]={"parser",cmd,NULL};
        if(execv("parser",argv)<0);exit(-1);
    }
    else /* pid!=0; parent process */
    {
        close(PARSER_READ_FD);
        close(PARSER_WRITE_FD);
        close(PROXY_WRITE_FD);

        *pid_ret=pid;
    }
    return PROXY_READ_FD;
}

/*
 *   Recieves a buffer and it's current size, the pid of the child Process and the File_Descriptor of the pipe
 *   Returns the pipes content in the buffer parameter and the new size of the buffer.
 *
 */
int read_parser(char * buffer, int size, int parser_pid, int proxy_read_fd)
{
    int status;
    waitpid(parser_pid,&status,0);
    if(WEXITSTATUS(status)!=GOOD_EXIT_STATUS)
    {
        perror("Parser failed, no content written");
        close(proxy_read_fd);
        return ERROR;
    }
    FILE * fstream = fdopen(proxy_read_fd, "r");

    size =fetchInputFromFile(&buffer,fstream, (size_t) size);

    if(size<0)
    {
        perror("read");
        return ERROR;
    }

    close(proxy_read_fd);
    return size;
}