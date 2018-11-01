#ifndef MEDIA_TYPES_H
#define MEDIA_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "mediaTypesTest.h"

#define TYPE_ARRAY_LENGTH 10
#define MAX_TYPE_SIZE 15
#define INITIAL_DICTIONARY_SIZE 5
#define INITIAL_INPUT_SIZE  100

#define buffer_size 100
#define BOUNDARIES 5


#define READ_LINE 0
#define WRITE_LINE 1
#define EVAL_MIME 2
#define BOUNDARY_SEARCH 3
#define WRITE_REPLACEMENT 4
#define FINISHED 5
#define WRITE_LINE_BEFORE_REPLACEMENT 6
#define READ_LINE_AFTER_REPLACEMENT 7

#define OK 1
#define MULTIPART 2
#define PROHIBITED 3


int isType(char * type);
int toNLowerString(char * lowerCaseCopy, char * original, int n);

char ** divideStrByDelimeter(char * string, char * delimeter);
char ** divideMediaType(char * mediaType);
char ** divideUserInputByLine(char * userInput);
char ** divideMediaRangeList(char * mediaTypeList);

int mediaTypeBelongsToMediaRange(char ** mediaType, char ** mediaRange);
void recursiveDoublePointerFree(char ** splitMediaType);
int isValidMediaType(char ** mediaType);
int fetchInputFromStdin(char ** bufferPosition);
int mediaRangeEvaluator(int argc, char ** args);
int media_type_state_machine(char ** media_type_complete_list, char * replacement_message, int replacement_message_size);


char *my_strdup(const char *s);
char *my_strsep(char ** string_ptr, char delimter );


#endif
