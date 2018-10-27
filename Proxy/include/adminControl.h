//
// Created by joaquin on 20/10/18.
//

#ifndef PROTOS_TPE_ADMINCONTROL_H
#define PROTOS_TPE_ADMINCONTROL_H
#include <stdlib.h>
#include "main.h"

typedef enum {
    AUTENTICACION, INTERCAMBIO, CIERRE
} estadoDeSesion;

typedef enum {
    USER, PASS, LISTS, STATS, ACTIVE, FILTER, QUIT
} comandos;

typedef struct {
    //cosas relacionadas con la conexion SCTP
    estadoDeSesion eSesion;

    char* usuario;
    char* contra;
    char** monitoreoArray;
} variablesDeSesion;

typedef variablesDeSesion * variablesDeSesion_p;



//Definiciones de tamaños
#define RESPONSE_SIZE sizeof(admin_response)
#define TAM_COMANDO sizeof(int)
#define EXITO 0
#define FALLO 1
#define ESPECIAL 2

//Funciones
void saludo();
int textResponseBS(int estadoDeRespuesta, char* contenido);
void procesarRequest();
int parseComando(char* resp);
variablesDeSesion_p inicializarVarSes();
int autenticar();
int getMonitoreoArray();
int monitoreo(int numero);
int getEstadoTransformacion();
int setEstadoTransformacion(int estado);
char* getFiltroTransformacion();
int setFiltroTransformacion(char* filtro);
void errorDeScope();
file_descriptor setup_admin_socket();
void errorDeFormato();
int parseMesaje(const char *str, char sep, char**comando, char** parametro);
void createConection();
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