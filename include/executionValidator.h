#ifndef ARGUMENTVALIDATOR_H
#define ARGUMENTVALIDATOR_H

#include <stdlib.h>
#include <stdio.h>
#include "main.h"

#define MAX_OPTIONS 10

typedef struct {
    char * name;
    char command;
    void (*function)();
    char * description;
    char selected;
} option_t;

void initialize_options();
void option_register(char * name, char command, void (*function)(), char * description);

void execute_options();

int validate_arguments(int argc, char ** argv);
int validate_argument(char * arg);
int multiple_option_validator(char * arg);
int option_validator(char option);

void test_mode();
void non_test_mode();

#endif //PROTOS_TPE_MAIN_H
