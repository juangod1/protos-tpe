#ifndef RUNCOMMANDLINE_H
#define RUNCOMMANDLINE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXPATH_LEN 255
#define INITIAL_INPUT_SIZE  100
#define READ_BUFFER_SIZE 10

#define NUM_PIPES          2

#define FILTER_READ_PIPE   0
#define FILTER_WRITE_PIPE  1

#define READ_FD  0
#define WRITE_FD 1

#define CONSUMER_READ_FD  ( pipes[FILTER_WRITE_PIPE][READ_FD]   )
#define PRODUCER_WRITE_FD ( pipes[FILTER_READ_PIPE][WRITE_FD] )

#define FILTER_READ_FD   ( pipes[FILTER_READ_PIPE][READ_FD]  )
#define FILTER_WRITE_FD  ( pipes[FILTER_WRITE_PIPE][WRITE_FD]  )

int fetchInputFromStdin(char ** bufferPosition);
void resetBuffer(char * buffer, int size);
void run_parser(char * command);

#endif
