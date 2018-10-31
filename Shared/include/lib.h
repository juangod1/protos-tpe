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

#define TYPE_ARRAY_LENGTH 10
#define MAX_TYPE_SIZE 15
#define INITIAL_DICTIONARY_SIZE 5

#define BAD_EXIT_STATUS 1
#define GOOD_EXIT_STATUS 0

int toNLowerString(char *lowerCaseCopy, char *original, int n);

char *my_strdup(const char *s);

char *my_strsep(char **string_ptr, char delimeter);

int isType(char *type);

char **divideStrByDelimeter(char *string, char delimeter);

char **divideMediaType(char *mediaType);

char **divideUserInputByLine(char *userInput);

char **divideMediaRangeList(char *mediaTypeList);

int mediaTypeBelongsToMediaRange(char **mediaType, char **mediaRange);

void recursiveDoublePointerFree(char **splitMediaType);

int isValidMediaType(char **mediaType);


size_t fetchInputFromStdin(char **bufferPosition, size_t size);

size_t fetchInputFromFile(char **bufferPosition, FILE *f, size_t size);

size_t fetchLineFromStdin(char **bufferPosition, size_t size);

size_t fetchLineFromFile(char **bufferPosition, FILE *f, size_t size);

#endif
