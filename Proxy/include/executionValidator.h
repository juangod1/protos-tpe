#ifndef ARGUMENTVALIDATOR_H
#define ARGUMENTVALIDATOR_H

#include <stdlib.h>
#include <stdio.h>
#include "main.h"

int validate_arguments(int argc, char ** argv);
int validate_argument(char * arg);
int multiple_option_validator(char * arg);

#endif
