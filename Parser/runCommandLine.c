#include "include/runCommandLine.h"
#include "include/parityByte.h"
//https://stackoverflow.com/questions/28507950/calling-ls-with-execv
//https://jineshkj.wordpress.com/2006/12/22/how-to-capture-stdin-stdout-and-stderr-of-child-program/

int pipes[NUM_PIPES][2];


/*
The Program executes 3 processes.
Producer Process reads from STDIN and sends input to FILTER_READ_PIPE.
Filter Process executes the bash command received, the command reads from FILTER_READ_PIPE and writes to FILTER_WRITE_PIPE
Consumer Process reads from FILTER_WRITE_PIPE and prints to STDOUT
*/
void run_parser(char * command)
{
  char * input;
  int size = fetchInputFromStdin(&input);

  //initialize pipes
  if(pipe(pipes[FILTER_READ_PIPE])==-1){
    perror("pipe");
    exit(-1);
  }
  if(pipe(pipes[FILTER_WRITE_PIPE])==-1){
    perror("pipe");
    exit(-1);
  }

  pid_t pid;

  pid = fork();

  if(pid==0){

    pid=fork();

    if(pid==0){ //FILTER PROCESS
      close(CONSUMER_READ_FD);
      close(PRODUCER_WRITE_FD);

      dup2(FILTER_READ_FD, STDIN_FILENO);
      dup2(FILTER_WRITE_FD, STDOUT_FILENO);

      close(FILTER_READ_FD);
      close(FILTER_WRITE_FD);

      int response = system(command); //executes command in parameter
      if(response<0)
      {
        printf("Could not execute command\n");
      }
    }
    else{ //PRODUCER PROCESS
      close(FILTER_READ_FD);
      close(FILTER_WRITE_FD);
      close(CONSUMER_READ_FD);

      if(write(PRODUCER_WRITE_FD, input, size)<0)
      {
        printf("Could not execute write\n");
        return;
      };

      char parity = parityByte(input, size);
      char * bytes = charToHex(parity);

      free(input);

      fprintf(stderr, "in parity: %s\n",bytes);

      close(PRODUCER_WRITE_FD);

    }
  }
  else{ //CONSUMER PROCESS
    close(PRODUCER_WRITE_FD);
    close(FILTER_READ_FD);
    close(FILTER_WRITE_FD);

    char buffer[READ_BUFFER_SIZE+1]={0};
    int readSize =0, size=0, allocsize=INITIAL_INPUT_SIZE;
    char * string = malloc(allocsize);

    while((readSize = read(CONSUMER_READ_FD,buffer,READ_BUFFER_SIZE))!=0){
      if(size>=allocsize){
        allocsize+=INITIAL_INPUT_SIZE;
        string = realloc(string, allocsize);
      }
      size+=readSize;

      buffer[READ_BUFFER_SIZE]=0;
      strcat(string,buffer);
      resetBuffer(buffer,READ_BUFFER_SIZE);
    }
    char parity = parityByte(string,size);
    char * hexString = charToHex(parity);


    fprintf(stderr, "out parity: %s\n",hexString);

    printf("%s\n",string);

    free(string);
    free(hexString);
    close(CONSUMER_READ_FD);
  }
}

int fetchInputFromStdin(char ** bufferPosition)
{
  char c;
  int counter=0;
  char * buffer = malloc(INITIAL_INPUT_SIZE);
  int size=INITIAL_INPUT_SIZE;
  while((c=getchar())!=EOF){
    if(counter>=size){
      size+=INITIAL_INPUT_SIZE;
      buffer = realloc(buffer, size);
    }
    *(buffer+counter)=c;
    counter++;
  }
  *bufferPosition=buffer;
  return counter;
}

void resetBuffer(char * buffer, int size)
{
  for(int i=0; i<size; i++){
    *(buffer+i)=0;
  }
}
