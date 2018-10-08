#ifndef ARGUMENTVALIDATOR_H
#define ARGUMENTVALIDATOR_H

#include <stdlib.h>
#include <stdio.h>
#include "main.h"

#define MAX_OPTIONS 10

typedef struct {
    char * name;
    char * command;
    void (*function)();
    char * description;
} option_t;

typedef struct {
    int length;
    int selectedOptions[MAX_OPTIONS];
} validation_response_t;

void initialize_options();
void option_register(char * name, char * command, void (*function)(), char * description);

void execute_options(int *selectedOptions, int optionLength);

validation_response_t validate_arguments(int argc, char ** argv);
int validate_argument(char * arg);
int option_validator(char option);

#endif //PROTOS_TPE_MAIN_H
