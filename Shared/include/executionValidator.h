#ifndef ARGUMENTVALIDATOR_H
#define ARGUMENTVALIDATOR_H

#include <stdlib.h>
#include <stdio.h>
#include "../../Proxy/include/main.h"

#define MAX_OPTIONS 12

typedef struct {
    char * name;
    char command;
    void (*function)();
    char * description;
    char selected;
} option_t;

int parse_command(int argc, char ** argv, void (*initialize_options)());
void execute_options();
int validate_arguments(int argc, char ** argv);
int validate_argument(char * arg);
int multiple_option_validator(char * arg);
void option_register(char * name, char command, void (*function)(), char * description);
int option_validator(char option);

#endif
