#ifndef MEDIA_TYPES_H
#define MEDIA_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "mediaTypesTest.h"


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

int mediaRangeEvaluator(char* replacementMessage, char* mediaList);
int media_type_state_machine(char ** media_type_complete_list, char * replacement_message, size_t replacement_message_size);


#endif
