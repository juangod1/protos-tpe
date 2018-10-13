#include "include/proxyCommunication.h"
#include "../Shared/include/lib.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int pipes[NUM_PIPES][2];

int start_parser(char * cmd,char * msg, size_t size)
{
    //initialize pipes
    if(pipe(pipes[PROXY_READ_PIPE])==-1)
    {
        perror("pipe");
        exit(-1);
    }
    if(pipe(pipes[PROXY_WRITE_PIPE])==-1)
    {
        perror("pipe");
        exit(-1);
    }

    if(write(PROXY_WRITE_FD,msg,size)==-1)
    {
        perror("write");
        exit(-1);
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
        execv("parser",argv);

        exit(-1);
    }
    else /* pid!=0; parent process */
    {
        close(PARSER_READ_FD);
        close(PARSER_WRITE_FD);
        close(PROXY_WRITE_FD);
    }
    return 0;
}

size_t read_parser(char * buffer, size_t size)
{
    FILE * fstream = fdopen(PROXY_READ_FD, "r");

    size =fetchInputFromFile(&buffer,fstream, size);

    if(size<0)
    {
        perror("read");
        exit(-1);
    }

    printf("'%s'\n",buffer);
    close(PROXY_READ_FD);
    free(buffer);
    return size;
}