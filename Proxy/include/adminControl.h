//
// Created by joaquin on 20/10/18.
//

#ifndef PROTOS_TPE_ADMINCONTROL_H
#define PROTOS_TPE_ADMINCONTROL_H

#include <stdlib.h>
#include "main.h"
#include "../../Shared/include/buffer.h"
#include "state.h"

typedef enum
{
	AUTENTICATION, EXCHANGE, CLOSE
} session_state;

typedef enum
{
	USER, PASS, LISTS, STATS, ACTIVE, FILTER, QUIT
} commands;


//Definiciones de tamaños
#define RESPONSE_SIZE sizeof(admin_response)
#define TAM_COMANDO sizeof(int)
#define SUCCESS 0
#define FAILED 1
#define ESPECIAL 2

#define SCOPE_ERROR text_response_BS(FAILED, "Command not available in state.", s, fd);
#define FORMAT_ERROR text_response_BS(FAILED, "Wrong command format.", s, fd);

//Funciones
int text_response_BS(int response_state, char *content, state s, file_descriptor fd);

void process_request(state s, file_descriptor fd);

void admin_greeting(state s, file_descriptor fd);

int parse_admin_command(const char *resp);

int authenticate(char *user, char *pass);

char **get_monitor_array();

long monitor(int numero);

int get_transformation_state();

void set_transformation_state(int estado, state s);

char *get_transformation_filter();

file_descriptor setup_admin_socket();

int parse_message(const char *str, char sep, char **comando, char **parametro);

int parse_positive_int(char *string);

#endif //PROTOS_TPE_ADMINCONTROL_H



//DEPRECATED
/*
 * typedef struct {
    char* comando;
    int estado;
    int tamcontent;
    char* content;
} admin_response;

typedef admin_response * admin_response_p;

typedef struct {
    char* comandoRel;
    int tamParametros;
    char* parametros;
} admin_request;

typedef admin_request * admin_request_p;


admin_request_p requestBuilder();
admin_response_p responseBuilder(char* comando, int estado, int tam, char* content);

*/