//
// Created by juangod on 08/10/18.
//

#ifndef PROTOS_TPE_OPTIONS_H
#define PROTOS_TPE_OPTIONS_H


#define MAX_OPTIONS 12

typedef struct {
    char * name;
    char command;
    void (*function)();
    char * description;
    char selected;
} option_t;


void initialize_options();
void option_register(char * name, char command, void (*function)(), char * description);
int option_validator(char option);

void execute_options();

void test_mode();
void non_test_mode();
void error_specification();
void help();
void management_direction();
void replacement_message();
void censored_mediatype();
void management_port();
void local_port();
void origin_port();
void command_specification();
void version();


#endif //PROTOS_TPE_OPTIONS_H
