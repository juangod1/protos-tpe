#include <stdbool.h>
#include "include/runCommandLine.h"
#include "include/parityByte.h"
#include "../Shared/include/lib.h"

//https://stackoverflow.com/questions/28507950/calling-ls-with-execv
//https://jineshkj.wordpress.com/2006/12/22/how-to-capture-stdin-stdout-and-stderr-of-child-program/

int pipes[NUM_PIPES][2];


/*
The Program executes 3 processes.
Producer Process reads from STDIN and sends input to FILTER_READ_PIPE.
Filter Process executes the bash command received, the command reads from FILTER_READ_PIPE and writes to FILTER_WRITE_PIPE
Consumer Process reads from FILTER_WRITE_PIPE and prints to STDOUT
*/
int run_parser(char * command)
{
  char * input = malloc(INITIAL_INPUT_SIZE);
  int counter = fetchInputFromStdin(&input, INITIAL_INPUT_SIZE);

  //initialize pipes
  if(pipe(pipes[FILTER_READ_PIPE])==-1){
    perror("pipe");
    exit(BAD_EXIT_STATUS);
  }
  if(pipe(pipes[FILTER_WRITE_PIPE])==-1){
    perror("pipe");
    exit(BAD_EXIT_STATUS);
  }

  pid_t pid;

  pid = fork();

  if(pid==0){

    pid=fork();

    if(pid==0){ //FILTER PROCESS
      close(CONSUMER_READ_FD);
      close(PRODUCER_WRITE_FD);
      free(input);

      dup2(FILTER_READ_FD, STDIN_FILENO);
      dup2(FILTER_WRITE_FD, STDOUT_FILENO);

      close(FILTER_READ_FD);
      close(FILTER_WRITE_FD);

      int status = system(command);
      if(WIFSTOPPED(status)==true && WSTOPSIG(status)==127)
      {
        perror("Invalid command");
        exit(BAD_EXIT_STATUS);
      }
      exit(GOOD_EXIT_STATUS);
    }
    else{ //PRODUCER PROCESS
      close(FILTER_READ_FD);
      close(FILTER_WRITE_FD);
      close(CONSUMER_READ_FD);

      if(write(PRODUCER_WRITE_FD, input, counter)<0)
      {
        printf("Could not execute write\n");
        close(PRODUCER_WRITE_FD);
        free(input);
        exit(BAD_EXIT_STATUS);
      };
      close(PRODUCER_WRITE_FD);

      int status;
      wait(&status);
      if(WIFEXITED(status)==false || WEXITSTATUS(status)!=GOOD_EXIT_STATUS)
      {
        free(input);
        exit(BAD_EXIT_STATUS);
      }

      char parity = parityByte(input, counter);
      char * bytes = charToHex(parity);

      fprintf(stderr, "in parity: %s\n",bytes);

      free(input);
      free(bytes);

      exit(GOOD_EXIT_STATUS);
    }
  }
  else{ //CONSUMER PROCESS
    close(PRODUCER_WRITE_FD);
    close(FILTER_READ_FD);
    close(FILTER_WRITE_FD);
    free(input);

    int status;
    wait(&status);
    if(WIFEXITED(status)==false || WEXITSTATUS(status)!=GOOD_EXIT_STATUS)
    {
      close(CONSUMER_READ_FD);
      return ERROR;
    }

    char * string = calloc(1,INITIAL_INPUT_SIZE);
    FILE * fstream = fdopen(CONSUMER_READ_FD, "r");

    size_t size =fetchInputFromFile(&string,fstream, INITIAL_INPUT_SIZE);

    char parity = parityByte(string,(int)size);
    char * hexString = charToHex(parity);

    fprintf(stderr, "out parity: %s\n",hexString);

    printf("%s",string);

    free(string);
    free(hexString);
    close(CONSUMER_READ_FD);
  }
  return 0;
}