//
// Created by joaquin on 17/10/18.
//
#include <memory.h>
#include <stdio.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include "include/adminControl.h"
#include "include/state.h"
#include "include/main.h"
#include "include/options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/sctp.h>

//El manejo de estar escuchando el socket correspondiente esta hecho por fuera de este archivo
//Este archivo va a manejar la comunicacion desde el lado del servidor cuando encuentra al menos un usuario administrador conectado

//Formatos de los mensajes
//REQUEST
//header: comando,tamaño
//body: parametros
//RESPONSE
//header: comando al que responde, estado de respuesta, tamaño
//body: contenido

//CAMBIOS A REALIZAR
//int parametros a ESTRUCTURA de parametros
#define MAX_BUFFER 1024
#define MY_PORT_NUM 9090

file_descriptor setup_admin_socket(){
    int listenSock, ret;
    struct sockaddr_in servaddr;
    struct sctp_initmsg initmsg;

    listenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if(listenSock == -1)
    {
        printf("Failed to create socket\n");
        perror("socket()");
        exit(1);
    }

    bzero ((void *) &servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port = htons (MY_PORT_NUM);

    ret = bind (listenSock, (struct sockaddr *) &servaddr, sizeof (servaddr));

    if(ret == -1 )
    {
        printf("Bind failed \n");
        perror("bind()");
        close(listenSock);
        exit(1);
    }

    /* Specify that a maximum of 5 streams will be available per socket */
    memset (&initmsg, 0, sizeof (initmsg));
    initmsg.sinit_num_ostreams = 5;
    initmsg.sinit_max_instreams = 5;
    initmsg.sinit_max_attempts = 4;
    ret = setsockopt (listenSock, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof (initmsg));

    if(ret == -1 )
    {
        printf("setsockopt() failed \n");
        perror("setsockopt()");
        close(listenSock);
        exit(1);
    }

    ret = listen (listenSock, 5);
    if(ret == -1 )
    {
        printf("listen() failed \n");
        perror("listen()");
        close(listenSock);
        exit(1);
    }

    /* Specify that a maximum of 5 streams will be available per socket */
    memset (&initmsg, 0, sizeof (initmsg));
    initmsg.sinit_num_ostreams = 5;
    initmsg.sinit_max_instreams = 5;
    initmsg.sinit_max_attempts = 4;
    ret = setsockopt (listenSock, IPPROTO_SCTP, SCTP_INITMSG,
                      &initmsg, sizeof (initmsg));

    if(ret == -1 )
    {
        printf("setsockopt() failed \n");
        perror("setsockopt()");
        close(listenSock);
        exit(1);
    }

    ret = listen (listenSock, 5);
    if(ret == -1 )
    {
        printf("listen() failed \n");
        perror("listen()");
        close(listenSock);
        exit(1);
    }

    return listenSock;
/*
    while (1)
    {

        char buffer[MAX_BUFFER + 1];
        int len;

        //Clear the buffer
        bzero (buffer, MAX_BUFFER + 1);

        printf ("Awaiting a new connection\n");


        else
            printf ("New client connected....\n");

        in = sctp_recvmsg (connSock, buffer, sizeof (buffer),
                           (struct sockaddr *) NULL, 0, &sndrcvinfo, &flags);

        if( in == -1)
        {
            printf("Error in sctp_recvmsg\n");
            perror("sctp_recvmsg()");
            close(connSock);
            continue;
        }
        else
        {
            //Add '\0' in case of text data
            buffer[in] = '\0';

            printf (" Length of Data received: %d\n", in);
            printf (" Data : %s\n", (char *) buffer);
        }
        close (connSock);
    }*/
}

int textResponseBS(int estadoDeRespuesta, char* contenido, buffer_p buffer, file_descriptor fd) {
    char res[100] = {0};
    char *ok = "+OK - ";
    char *err = "-ERR - ";
    char *esp = "* - ";
    if (estadoDeRespuesta == 0) {
        strcat(res, ok);
    } else if (estadoDeRespuesta == 1) {
        strcat(res, err);
    } else {
        //Estado especial
        strcat(res, esp);
    }
    //Enviar el la respuesta
    strcat(res, contenido);
    strcat(res, "\n");
    int amount = (double)strlen(res)/(double)BUFFER_SIZE + ((((int)strlen(res)%(int)BUFFER_SIZE)==0)?0:1);
    for(int i =0; i < amount; i++)
    {
        buffer_read_string(res+(i*BUFFER_SIZE), buffer);
        buffer_write(fd, buffer);
    }
    return  0;
}


int parseMesaje(const char *str, char sep, char**comando, char** parametro)
{
    int cuenta =0;
    unsigned int start = 0, stop;
    for (stop = 0; str[stop]; stop++) {
        if (str[stop] == sep) {
            if(cuenta == 1)
                return 1;
            *comando = calloc(1,stop-start+1);//Por ahi me falta 1 para finalizar el string
            memcpy(*comando, str + start, stop - start);
            cuenta++;
            start = stop + 1;
        }
    }
    if(cuenta == 0){
        *comando = calloc(1,stop-start+1);//Por ahi me falta 1 para finalizar el string
        memcpy(*comando, str + start, stop - start);
        cuenta++;
        start = stop + 1;
        return 0;
    }
    *parametro = calloc(1,stop-start+1);
    memcpy(*parametro, str + start, stop - start);//Por ahi me falta uno para finalizar el string
    return 0;
}

void procesarRequest(state s, file_descriptor fd){
    buffer_p buffer = s->buffers[0];
    char respuesta[MAX_BUFFER] = {0};
    buffer_write_string(respuesta,buffer);
    printf("%s",respuesta);
    //Separo el string de respuesta por espacios y analizo cada cosa.
    char *comando = NULL;
    char *parametro = NULL;
    if(parseMesaje(respuesta, ' ', &comando, &parametro) == 1){
        //Error de mensaje
        textResponseBS(FALLO,"Message format error.", buffer, fd);
        //TODO:Puedo cerrar la conexion o dejar que siga un poco mas.
    }
    switch(parseComando(comando)){
        case USER:
            if(s->protocol_state != AUTENTICACION){
                //Error de SCOPE
                SCOPE_ERROR
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
                break;
            }
            //Se procede
            if(parametro != NULL){
                s->user = calloc(1, strlen(parametro) + 1);
                memcpy(s->user, parametro, strlen(parametro));
                textResponseBS(EXITO,"Continue with PASS.", buffer, fd);
            } else {
                textResponseBS(FALLO, "Missing argument", buffer, fd);
            }
            break;
        case PASS:
            if(s->protocol_state != AUTENTICACION){
                //Error de SCOPE
                SCOPE_ERROR
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
                break;
            }
            //Tiene que haber ingresado un usuario previamente
            if(s->user != NULL){
                s->pass = calloc(1, strlen(parametro) + 1);
                memcpy(s->pass, parametro, strlen(parametro));
                int auth = autenticar(s->user,s->pass);
                if(auth == 0){
                    textResponseBS(EXITO, "Entering EXCHAGE.", buffer, fd);
                    s->protocol_state = INTERCAMBIO;
                } else if(auth == 1){
                    textResponseBS(FALLO, "FAILED. Try again or QUIT.", buffer, fd);
                } else {
                    textResponseBS(FALLO, "Conection error, try asgain later.", buffer, fd);
                }
            } else {
                textResponseBS(FALLO, "Missing USER.", buffer, fd);
            }
            break;
        case LISTS:
            if(s->protocol_state != INTERCAMBIO){
                //Error de SCOPE
                SCOPE_ERROR
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
                break;
            }
            if(parametro != NULL){
                //Si hay parametros, presentar un ERROR DE FORMATO
                FORMAT_ERROR
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            } else {
                char contenido[100] = {0};
                strcat(contenido, "List:");
                char** monitoreoArray = getMonitoreoArray();
                for(int i = 0; i<5;i++){
                    strcat(contenido, "\n");
                    strcat(contenido, monitoreoArray[i]);
                }
                textResponseBS(1, contenido, buffer, fd);
            }
            break;
        case STATS:
            if(s->protocol_state != INTERCAMBIO){
                //Error de SCOPE
                SCOPE_ERROR
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            int paramNum = parsePosInt(parametro);
            if(paramNum == -1){
                //Si no es un numero presentar un ERROR DE FORMATO
                FORMAT_ERROR
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            int resMonitoreo = monitoreo(paramNum);
            if(resMonitoreo == -1){
                textResponseBS(FALLO,"Function not found, use LISTS.", buffer, fd);
            } else {
                char textoMonitoreo[15];
                sprintf(textoMonitoreo, "%d", resMonitoreo);
                //Aca se puede hacer referencia a la funcion o al numero de funcion que fue llamado.
                textResponseBS(EXITO, strcat("El resultdo es ", textoMonitoreo), buffer, fd);//TODO: Hay que acomodar el strcat
            }
            break;
        case ACTIVE:
            if(s->protocol_state != INTERCAMBIO){
                //Error de SCOPE
                SCOPE_ERROR
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            if(parametro == NULL){
                //Significa que no paso parametro entonces quiere saber cual es el estado actual
                int estadoT = getEstadoTransformacion();
                textResponseBS(EXITO, strcat("Transformation is: ", (estadoT ? "Active":"Inactive")), buffer, fd);
            } else {
                int paramNum = parsePosInt(parametro);
                if ( paramNum != 1 && paramNum != 0 || paramNum == -1){
                    //ERROR DE FORMATO - parametros invalidos
                    FORMAT_ERROR
                    //TODO:Cerrar la respuesta y solicitar un nuevo request.
                }
                if(setEstadoTransformacion(paramNum) == 0) {
                    textResponseBS(EXITO, strcat("SUCCESS. Transformation: ",
                                                 (paramNum ? "Active" : "Inactive")), buffer, fd);
                } else {
                    //ERROR Interno
                    //TODO: Manejar errores internos y solicitar un nuevo request.
                }
            }
            break;
        case FILTER:
            if(s->protocol_state != INTERCAMBIO){
                //Error de SCOPE
                SCOPE_ERROR
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            if(parametro == NULL){
                //Significa que no paso parametro entonces quiere saber cual es el filter actual
                char* filtro = getFiltroTransformacion();
                textResponseBS(EXITO, strcat("Current transformation: ", filtro), buffer, fd);
            } else {
                if(setFiltroTransformacion(parametro) == 0) {
                    textResponseBS(EXITO, strcat("SUCCESS. Current transformation: ", parametro), buffer, fd);
                } else {
                    //ERROR Interno
                    //TODO: Manejar errores internos y solicitar un nuevo request.
                }
            }
            break;
        case QUIT:
            s->protocol_state = CIERRE;
            //Es multiestado entonces no verifico
            textResponseBS(EXITO, "Goodbye!", buffer, fd);
            //cerrar sesion cerrando el socket y dropeando la informacion de sesion
            break;
        //Y por ultimo tenemos el caso default por si falla por scope o por error
        default:
            //El comando ingresado no esta dentro de los disponibles.
            textResponseBS(FALLO, "Command unknown. Refer to the RFC.", buffer, fd);
            break;
    }
}

int parseComando(const char* resp){
    if(resp == NULL){
        return -1;
    }
    if(strcmp(resp, "USER") == 0)
        return 0;
    if(strcmp(resp, "PASS") == 0)
        return 1;
    if(strcmp(resp, "LISTS") == 0)
        return 2;
    if(strcmp(resp, "STATS") == 0)
        return 3;
    if(strcmp(resp, "ACTIVE") == 0)
        return 4;
    if(strcmp(resp, "FILTER") == 0)
        return 5;
    if(strcmp(resp, "QUIT") == 0)
        return 6;
    return -1;
}



int autenticar(char *user, char* pass){
    //Usando las variable globales tiene que autenticar y luego proporcionar el codigo que resulto
    char *users[2] = {"pablito","juancito"};
    char *passes[2] = {"pabs", "juans"};
    for(int i=0;i<2;i++){
        if(strcmp(users[i],user) == 0){
            if(strcmp(passes[i], pass) == 0){
                return 0;
            }
            return 1;
        }
    }
    return 1;
}

//Retorna -1 si falla
int parsePosInt(char* string){
    char* remain;
    long ret = strtol(string, &remain,10);
    if(*remain != '\0'){
        //Hubo un error
        return -1;
    }
    return (int)ret;
}

//Esta funcion tiene que estar hecha en el proxy exclusivamente y tiene que cargar
//el array de monitoreo con strings que digan su numero espacio funcion ej: "1 usuariosOnline"
//Devuelve el numero de funciones que hay
char** getMonitoreoArray(){
    app_context_p app = get_app_context();
    return app->monitor;
}

//Busca la estadistica correspondiente al monitoreo(i)
int monitoreo(int numero){
    //getMonitoreoContext con el numero
    return 0;
}

//Busca en el contexto del proxy el estado actual de la transformacion. ACTIVO o INACTIVO
int getEstadoTransformacion(){

    return 0;
}

//Carga el estado de transformacion y devuelve 0 si funciono y -1 sino.
int setEstadoTransformacion(int estado){

    return 0;
}

//Busca el comando actual en el contexto del proxy
char* getFiltroTransformacion(){

    return "";
}

//Carga el filtro que le pasan retorna 0 si funciono y -1 sino
int setFiltroTransformacion(char* filtro){

    return 0;
}

//DEPRECATED
/*
admin_response_p requestBuilder(){
    //Recibo el mensaje de sctp
    //sctp_recvmsg
    //Si devuelve -1 doy error
    //Sino armo el admin_response_p
    if(comando == NULL || !(estado == 0 || estado == 1) || tam < 0){
        return NULL;
    }
    admin_response_p res = malloc(RESPONSE_SIZE);
    if(res == NULL) {
        //Mensaje de error
        return NULL;
    }
    res->comando = malloc(TAM_COMANDO);
    if(res->comando == NULL) {
        //Mensaje de error
        return NULL;
    }
    strcpy(res->comando, comando);
    res->estado = estado;
    res->tamContenido = tam;
    res->contenido = malloc(tam);
    if(res->contenido == NULL) {
        //Mensaje de error
        return NULL;
    }
    strcpy(res->contenido, contenido);
    return res;
}

admin_response_p responseBuilder(char* comando, int estado, int tam, char* contenido){
    if(comando == NULL || !(estado == 0 || estado == 1) || tam < 0){
        return NULL;
    }
    admin_response_p res = malloc(RESPONSE_SIZE);
    if(res == NULL) {
        //Mensaje de error
        return NULL;
    }
    res->comando = malloc(TAM_COMANDO);
    if(res->comando == NULL) {
        //Mensaje de error
        return NULL;
    }
    strcpy(res->comando, comando);
    res->estado = estado;
    res->tamContenido = tam;
    res->contenido = malloc(tam);
    if(res->contenido == NULL) {
        //Mensaje de error
        return NULL;
    }
    strcpy(res->contenido, contenido);
    return res;
}

 */