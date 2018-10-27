//
// Created by joaquin on 17/10/18.
//
#include <memory.h>
#include <stdio.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include "include/adminControl.h"
#include "include/main.h"
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

variablesDeSesion_p varSes;

//Funcion general. Una vez que se establece una conexion sctp con el puerto
void successfullConection(int parametros){

    //Creo todo el contexto de conexion, los sockets y las variables globales;
    varSes = inicializarVarSes();

    //Dado que conecto exitosamente hago el greeting
    saludo(parametros);

    procesarRequest();
}

file_descriptor setup_admin_socket(){
    int listenSock, connSock, ret, in, flags, i;
    struct sockaddr_in servaddr;
    struct sctp_initmsg initmsg;
    struct sctp_event_subscribe events;
    struct sctp_sndrcvinfo sndrcvinfo;
    char buffer[MAX_BUFFER + 1];

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

void saludo(int parametros){
    //Envio un saludo a traves de la conexion SCTP
    //"Conexion establecida!";
    char* mensaje = "Conexion establecida!";
    //Enviar por conexion la respuesta.
    textResponseBS(ESPECIAL,"Conexion establecida! Saludos!");
}

int textResponseBS(int estadoDeRespuesta, char* contenido){
    char* res;
    strcat(contenido, ".\r\n");
    if(estadoDeRespuesta == 1){
        res = strcat("+OK - ", contenido);
    } else if(estadoDeRespuesta == 0) {
        res = strcat("-ERR - ", contenido);
    } else {
        //Estado especial
        res = strcat("* - ", contenido);
    }
    //Enviar el la respuesta
    if(1) {//sctp_sendmsg(1,(void *)mensaje, sizeof(mensaje),to,tolen,ppid,flags,strem_no,timetolive,context);
        //Si sale mal, retorno -1, sino 0
        return -1;
    }
    return  0;

}

variablesDeSesion_p inicializarVarSes(){
    variablesDeSesion_p var = malloc(sizeof(variablesDeSesion));
    var->eSesion = AUTENTICACION;
    //Completar las demas cosas
    var->usuario = NULL;
}

int parseMesaje(const char *str, char sep, char**comando, char** parametro)
{
    int cuenta =0;
    unsigned int start = 0, stop;
    for (stop = 0; str[stop]; stop++) {
        if (str[stop] == sep) {
            if(cuenta == 1)
                return 1;
            *comando = calloc(1,stop-start);//Por ahi me falta 1 para finalizar el string
            memcpy(*comando, str + start, stop - start);
            cuenta++;
            start = stop + 1;
        }
    }
    *parametro = calloc(1,stop-start);
    memcpy(*parametro, str + start, stop - start);//Por ahi me falta uno para finalizar el string
    return 0;
}

void procesarRequest(){
    //Esta funcion tiene que:
    //-recibir la request
    //-parsear la request
    //-definir si es correcta en cuanto a forma y contexto
    //-consultar la respuesta
    //-generar la response

    //Recibir la request
    char * respuesta;//sctp_recvmsg(1,(void *)mensaje, sizeof(mensaje),to,tolen,ppid,flags,strem_no,timetolive,context);
    //Separo el string de respuesta por espacios y analizo cada cosa.
    char *comando = NULL;
    char *parametro = NULL;
    if(parseMesaje(respuesta, ' ', &comando, &parametro) == 1){
        //Error de mensaje
        textResponseBS(FALLO,"El mensaje enviado contiene un error de formato.");
        //TODO:Puedo cerrar la conexion o dejar que siga un poco mas.
    }
    switch(parseComando(comando)){
        case USER:
            if(varSes->eSesion != AUTENTICACION){
                //Error de SCOPE
                errorDeScope();
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            //Se procede
            if(parametro != NULL){
                varSes->usuario = parametro;//Este tiene que ser el 2do string del mensaje enviado
            }
            textResponseBS(EXITO,"Continue con PASS");
            break;
        case PASS:
            if(varSes->eSesion != AUTENTICACION){
                //Error de SCOPE
                errorDeScope();
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            //Tiene que haber ingresado un usuario previamente
            if(varSes->usuario != NULL){
                varSes->contra = parametro;
                int auth = autenticar();
                if(auth == 0){
                    textResponseBS(EXITO, "Autenticacion exitosa, se encuentra en INTERCAMBIO");
                    varSes->eSesion = INTERCAMBIO;
                } else if(auth == 1){
                    textResponseBS(FALLO, "Fallo la autenticacion. Intente nuevamente o finalize con QUIT");
                } else {
                    textResponseBS(FALLO, "El error producido fue de conexion, intente mas tarde");
                }
            } else {
                textResponseBS(FALLO, "Tiene que proporcionar un usuario primero");
            }
            break;
        case LISTS:
            if(varSes->eSesion != INTERCAMBIO){
                //Error de SCOPE
                errorDeScope();
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            if(parametro != NULL){
                //Si hay parametros, presentar un ERROR DE FORMATO
                errorDeFormato();
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            char* contenido = "Lista";
            for(int i = 0; i<getMonitoreoArray();i++){
                strcat(contenido, "\n");
                strcat(contenido, varSes->monitoreoArray[i]);
            }
            textResponseBS(1, contenido);
            break;
        case STATS:
            if(varSes->eSesion != INTERCAMBIO){
                //Error de SCOPE
                errorDeScope();
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            int paramNum = parsePosInt(parametro);
            if(paramNum == -1){
                //Si no es un numero presentar un ERROR DE FORMATO
                errorDeFormato();
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            int resMonitoreo = monitoreo(paramNum);
            if(resMonitoreo == -1){
                textResponseBS(FALLO,"No existe la funcion solicitada. Utilizar LISTS para mas informacion.");
            } else {
                char textoMonitoreo[15];
                sprintf(textoMonitoreo, "%d", resMonitoreo);
                //Aca se puede hacer referencia a la funcion o al numero de funcion que fue llamado.
                textResponseBS(EXITO, strcat("El resultdo es ", textoMonitoreo));
            }
            break;
        case ACTIVE:
            if(varSes->eSesion != INTERCAMBIO){
                //Error de SCOPE
                errorDeScope();
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            if(parametro == NULL){
                //Significa que no paso parametro entonces quiere saber cual es el estado actual
                int estadoT = getEstadoTransformacion();
                textResponseBS(EXITO, strcat("La transformacion se encuentra: ", (estadoT ? "Activa":"Inactiva")));
            } else {
                int paramNum = parsePosInt(parametro);
                if ( paramNum != 1 && paramNum != 0 || paramNum == -1){
                    //ERROR DE FORMATO - parametros invalidos
                    errorDeFormato();
                    //TODO:Cerrar la respuesta y solicitar un nuevo request.
                }
                if(setEstadoTransformacion(paramNum) == 0) {
                    textResponseBS(EXITO, strcat("El cambio se realizo con exito. Transformacion: ",
                                                 (paramNum ? "Activa" : "Inactiva")));
                } else {
                    //ERROR Interno
                    //TODO: Manejar errores internos y solicitar un nuevo request.
                }
            }
            break;
        case FILTER:
            if(varSes->eSesion != INTERCAMBIO){
                //Error de SCOPE
                errorDeScope();
                //TODO:Cerrar la respuesta y solicitar un nuevo request.
            }
            if(parametro == NULL){
                //Significa que no paso parametro entonces quiere saber cual es el filter actual
                char* filtro = getFiltroTransformacion();
                textResponseBS(EXITO, strcat("La transformacion actual es: ", filtro));
            } else {
                if(setFiltroTransformacion(parametro) == 0) {
                    textResponseBS(EXITO, strcat("El cambio se realizo con exito. Transformacion: ", parametro));
                } else {
                    //ERROR Interno
                    //TODO: Manejar errores internos y solicitar un nuevo request.
                }
            }
            break;
        case QUIT:
            varSes->eSesion = CIERRE;
            //Es multiestado entonces no verifico
            textResponseBS(EXITO, "Sesion finalizada");
            //cerrar sesion cerrando el socket y dropeando la informacion de sesion
            break;
        //Y por ultimo tenemos el caso default por si falla por scope o por error
        default:
            //El comando ingresado no esta dentro de los disponibles.
            textResponseBS(FALLO, "El comando no exite. Leer RFC para ver los preestablecidos");
            break;
    }
}

int parseComando(char* resp){
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

void errorDeScope(){
    textResponseBS(FALLO, "El comando no se encuentra disponible en este estado de sesion.");
}

void errorDeFormato(){
    textResponseBS(FALLO, "El ultimo mensaje no presenta el formato correcto.");
}

int autenticar(){
    //Usando las variable globales tiene que autenticar y luego proporcionar el codigo que resulto
    return 0;
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
int getMonitoreoArray(){
    //getMonitoreoContext
    return 0;
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