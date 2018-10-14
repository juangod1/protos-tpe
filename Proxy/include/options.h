//
// Created by juangod on 08/10/18.
//

#ifndef PROTOS_TPE_OPTIONS_H
#define PROTOS_TPE_OPTIONS_H

typedef struct {
    char * error_path;
    int pop3_path;
    int management_path;
    char * replacement_message;
    char ** censored_media_types;
    int management_port;
    int local_port;
    int origin_port;
    char * command_specification;
} app_context_t;

typedef app_context_t * app_context_p;


void initialize_options();
void initialize_app_context();
app_context_p get_app_context();
void destroy_app_context();

void pop3_direction(char * arg);
void error_specification(char * arg);
void management_direction(char * arg);
void replacement_message(char * arg);
void censored_mediatype(char * arg);
void management_port(char * arg);
void local_port(char * arg);
void origin_port(char * arg);
void command_specification(char * arg);

void help();
void version();


#endif //PROTOS_TPE_OPTIONS_H
