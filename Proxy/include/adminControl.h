//
// Created by joaquin on 20/10/18.
//

#ifndef PROTOS_TPE_ADMINCONTROL_H
#define PROTOS_TPE_ADMINCONTROL_H
#include <stdlib.h>
#include "main.h"
#include "../../Shared/include/buffer.h"
#include "state.h"

typedef enum {
    AUTENTICACION, INTERCAMBIO, CIERRE
} estadoDeSesion;

typedef enum {
    USER, PASS, LISTS, STATS, ACTIVE, FILTER, QUIT
} comandos;


//Definiciones de tamaños
#define RESPONSE_SIZE sizeof(admin_response)
#define TAM_COMANDO sizeof(int)
#define EXITO 0
#define FALLO 1
#define ESPECIAL 2

#define SCOPE_ERROR textResponseBS(FALLO, "Commnad not available in state.", buffer, fd);
#define FORMAT_ERROR textResponseBS(FALLO, "Wrong command format.", buffer, fd);

//Funciones
int textResponseBS(int estadoDeRespuesta, char* contenido, buffer_p buffer, file_descriptor fd);
void procesarRequest(state s, file_descriptor fd);
int parseComando(const char* resp);
int autenticar(char *user, char *pass);
char ** getMonitoreoArray();
int monitoreo(int numero);
int getEstadoTransformacion();
int setEstadoTransformacion(int estado);
char* getFiltroTransformacion();
int setFiltroTransformacion(char* filtro);
file_descriptor setup_admin_socket();
int parseMesaje(const char *str, char sep, char**comando, char** parametro);
int parsePosInt(char* string);
#endif //PROTOS_TPE_ADMINCONTROL_H



//DEPRECATED
/*
 * typedef struct {
    char* comando;
    int estado;
    int tamContenido;
    char* contenido;
} admin_response;

typedef admin_response * admin_response_p;

typedef struct {
    char* comandoRel;
    int tamParametros;
    char* parametros;
} admin_request;

typedef admin_request * admin_request_p;


admin_request_p requestBuilder();
admin_response_p responseBuilder(char* comando, int estado, int tam, char* contenido);

*/