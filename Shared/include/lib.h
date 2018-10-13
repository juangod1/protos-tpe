#ifndef LIB_H
#define LIB_H


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ERROR -1
#define STANDARD 0
#define VERSION 1
#define HELP 2

#define INITIAL_INPUT_SIZE  100


int fetchInputFromStdin(char ** bufferPosition, size_t size);
int fetchInputFromFile(char ** bufferPosition, FILE * f, size_t  size);
int fetchLineFromStdin(char ** bufferPosition, size_t  size);

#endif
