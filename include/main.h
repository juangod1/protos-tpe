//
// Created by juangod on 04/10/18.
//

#ifndef PROTOS_TPE_MAIN_H
#define PROTOS_TPE_MAIN_H

#define MAX_OPTIONS 10

typedef struct {
    char * name;
    char * command;
    void (*function)();
    char * description;
} option_t;

int argument_validator(char arg);
void test_mode();
void non_test_mode();
void option_register(char * name, char * command, void (*function)(), char * description);
void initialize_options();

#endif //PROTOS_TPE_MAIN_H
