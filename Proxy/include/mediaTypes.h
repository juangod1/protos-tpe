#ifndef MEDIA_TYPES_H
#define MEDIA_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NOT_INCLUDED_MEDIATYPE 0
#define INCLUDED_MEDIATYPE 1
#define INVALID_MEDIATYPE 2

int mediaRangeEvaluator(char ** media_range_complete_list, char * media_type);


#endif
