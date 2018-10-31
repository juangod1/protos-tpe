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
#include "include/error.h"
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
//header: command,tamaño
//body: parameters
//RESPONSE
//header: command al que responde, estado de response, tamaño
//body: content

//CAMBIOS A REALIZAR
//int parameters a ESTRUCTURA de parameters
#define MAX_BUFFER 1024
#define MY_PORT_NUM 9090

file_descriptor setup_admin_socket()
{
	int                 listenSock, ret;
	struct sockaddr_in  servaddr;
	struct sctp_initmsg initmsg;

	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if(listenSock == -1)
	{
		printf("Failed to create socket\n");
		perror("socket()");
		error_terminal();
	}

	bzero((void *) &servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(MY_PORT_NUM);

	ret = bind(listenSock, (struct sockaddr *) &servaddr, sizeof(servaddr));

	if(ret == -1)
	{
		printf("Bind failed \n");
		perror("bind()");
		close(listenSock);
		error_terminal();
	}

	/* Specify that a maximum of 5 streams will be available per socket */
	memset(&initmsg, 0, sizeof(initmsg));
	initmsg.sinit_num_ostreams  = 5;
	initmsg.sinit_max_instreams = 5;
	initmsg.sinit_max_attempts  = 4;
	ret = setsockopt(listenSock, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));

	if(ret == -1)
	{
		printf("setsockopt() failed \n");
		perror("setsockopt()");
		close(listenSock);
		error_terminal();
	}

	ret = listen(listenSock, 5);
	if(ret == -1)
	{
		printf("listen() failed \n");
		perror("listen()");
		close(listenSock);
		error_terminal();
	}

	/* Specify that a maximum of 5 streams will be available per socket */
	memset(&initmsg, 0, sizeof(initmsg));
	initmsg.sinit_num_ostreams  = 5;
	initmsg.sinit_max_instreams = 5;
	initmsg.sinit_max_attempts  = 4;
	ret = setsockopt(listenSock, IPPROTO_SCTP, SCTP_INITMSG,
	                 &initmsg, sizeof(initmsg));

	if(ret == -1)
	{
		printf("setsockopt() failed \n");
		perror("setsockopt()");
		close(listenSock);
		error_terminal();
	}

	ret = listen(listenSock, 5);
	if(ret == -1)
	{
		printf("listen() failed \n");
		perror("listen()");
		close(listenSock);
		error_terminal();
	}

	return listenSock;
}

int text_response_BS(int response_state, char *content, buffer_p buffer, file_descriptor fd)
{
	char res[100] = {0};
	char *ok      = "+OK - ";
	char *err     = "-ERR - ";
	char *esp     = "* - ";
	if(response_state == 0)
	{
		strcat(res, ok);
	}
	else if(response_state == 1)
	{
		strcat(res, err);
	}
	else
	{
		//Estado especial
		strcat(res, esp);
	}
	//Enviar el la response
	strcat(res, content);
	strcat(res, "\n");
	int     amount = (double) strlen(res) / (double) BUFFER_SIZE +
	                 ((((int) strlen(res) % (int) BUFFER_SIZE) == 0) ? 0 : 1);
	for(int i      = 0; i < amount; i++)
	{
		buffer_read_string(res + (i * BUFFER_SIZE), buffer);
		buffer_write(fd, buffer);
	}
	return 0;
}


int parse_message(const char *str, char sep, char **command, char **parameter)
{
	int          count = 0;
	unsigned int start = 0, stop;
	for(stop = 0; str[stop]; stop++)
	{
		if(str[stop] == sep)
		{
			if(count == 1)
			{
				return 1;
			}
			*command = calloc(1, stop - start + 1);//Por ahi me falta 1 para finalizar el string
			memcpy(*command, str + start, stop - start);
			count++;
			start = stop + 1;
		}
	}
	if(count == 0)
	{
		*command = calloc(1, stop - start + 1);//Por ahi me falta 1 para finalizar el string
		memcpy(*command, str + start, stop - start);
		count++;
		start = stop + 1;
		return 0;
	}
	*parameter = calloc(1, stop - start + 1);
	memcpy(*parameter, str + start, stop - start);//Por ahi me falta uno para finalizar el string
	return 0;
}

void process_request(state s, file_descriptor fd)
{
	buffer_p buffer               = s->buffers[0];
	char     response[MAX_BUFFER] = {0};
	buffer_write_string(response, buffer);
	printf("%s", response);
	//Separo el string de response por espacios y analizo cada cosa.
	char *command   = NULL;
	char *parameter = NULL;
	if(parse_message(response, ' ', &command, &parameter) == 1)
	{
		//Error de mensaje
		text_response_BS(FAILED, "Message format error.", buffer, fd);
		//TODO:Puedo cerrar la conexion o dejar que siga un poco mas.
	}
	switch(parse_admin_command(command))
	{
		case USER:
			if(s->protocol_state != AUTENTICATION)
			{
				//Error de SCOPE
				SCOPE_ERROR
				//TODO:Cerrar la response y solicitar un nuevo request.
				break;
			}
			//Se procede
			if(parameter != NULL)
			{
				s->user = calloc(1, strlen(parameter) + 1);
				memcpy(s->user, parameter, strlen(parameter));
				text_response_BS(SUCCESS, "Continue with PASS.", buffer, fd);
			}
			else
			{
				text_response_BS(FAILED, "Missing argument", buffer, fd);
			}
			break;
		case PASS:
			if(s->protocol_state != AUTENTICATION)
			{
				//Error de SCOPE
				SCOPE_ERROR
				//TODO:Cerrar la response y solicitar un nuevo request.
				break;
			}
			//Tiene que haber ingresado un usuario previamente
			if(s->user != NULL)
			{
				s->pass = calloc(1, strlen(parameter) + 1);
				memcpy(s->pass, parameter, strlen(parameter));
				int auth = authenticate(s->user, s->pass);
				if(auth == 0)
				{
					text_response_BS(SUCCESS, "Entering EXCHAGE.", buffer, fd);
					s->protocol_state = EXCHANGE;
				}
				else if(auth == 1)
				{
					text_response_BS(FAILED, "FAILED. Try again or QUIT.", buffer, fd);
				}
				else
				{
					text_response_BS(FAILED, "Conection error, try asgain later.", buffer, fd);
				}
			}
			else
			{
				text_response_BS(FAILED, "Missing USER.", buffer, fd);
			}
			break;
		case LISTS:
			if(s->protocol_state != EXCHANGE)
			{
				//Error de SCOPE
				SCOPE_ERROR
				//TODO:Cerrar la response y solicitar un nuevo request.
				break;
			}
			if(parameter != NULL)
			{
				//Si hay parameters, presentar un ERROR DE FORMATO
				FORMAT_ERROR
				//TODO:Cerrar la response y solicitar un nuevo request.
			}
			else
			{
				char content[100] = {0};
				strcat(content, "List:");
				char    **monitorArray = get_monitor_array();
				for(int i              = 0; i < 5; i++)
				{
					strcat(content, "\n");
					strcat(content, monitorArray[i]);
				}
				text_response_BS(1, content, buffer, fd);
			}
			break;
		case STATS:
			if(s->protocol_state != EXCHANGE)
			{
				//Error de SCOPE
				SCOPE_ERROR
				//TODO:Cerrar la response y solicitar un nuevo request.
			}
			else if(parameter != NULL)
			{
				int paramNum = parse_positive_int(parameter);
				if(paramNum == -1)
				{
					//Si no es un numero presentar un ERROR DE FORMATO
					FORMAT_ERROR
					//TODO:Cerrar la response y solicitar un nuevo request.
				}
				else
				{
					int resmonitor = monitor(paramNum);
					if(resmonitor == -1)
					{
						text_response_BS(FAILED, "Function not found, use LISTS.", buffer, fd);
					}
					else
					{
						char textomonitor[5];
						char content[50] = "El resultdo es ";
						sprintf(textomonitor, "%d", resmonitor);
						//Aca se puede hacer referencia a la funcion o al numero de funcion que fue llamado.
						text_response_BS(SUCCESS, strcat(content, textomonitor), buffer,
						                 fd);//TODO: Hay que acomodar el strcat
					}
				}
			}
			else
			{
				FORMAT_ERROR
			}
			break;
		case ACTIVE:
			if(s->protocol_state != EXCHANGE)
			{
				//Error de SCOPE
				SCOPE_ERROR
				//TODO:Cerrar la response y solicitar un nuevo request.
			}
			if(parameter == NULL)
			{
				//Significa que no paso parameter entonces quiere saber cual es el estado actual
				int estadoT = get_transformation_state();
				text_response_BS(SUCCESS, strcat("Transformation is: ", (estadoT ? "Active" : "Inactive")), buffer, fd);
			}
			else
			{
				int paramNum = parse_positive_int(parameter);
				if(paramNum != 1 && paramNum != 0 || paramNum == -1)
				{
					//ERROR DE FORMATO - parameters invalidos
					FORMAT_ERROR
					//TODO:Cerrar la response y solicitar un nuevo request.
				}
				if(set_transformation_state(paramNum) == 0)
				{
					text_response_BS(SUCCESS, strcat("SUCCESS. Transformation: ",
					                                 (paramNum ? "Active" : "Inactive")), buffer, fd);
				}
				else
				{
					//ERROR Interno
					//TODO: Manejar errores internos y solicitar un nuevo request.
				}
			}
			break;
		case FILTER:
			if(s->protocol_state != EXCHANGE)
			{
				//Error de SCOPE
				SCOPE_ERROR
				//TODO:Cerrar la response y solicitar un nuevo request.
			}
			if(parameter == NULL)
			{
				//Significa que no paso parameter entonces quiere saber cual es el filter actual
				char *filtro = get_transformation_filter();
				text_response_BS(SUCCESS, strcat("Current transformation: ", filtro), buffer, fd);
			}
			else
			{
				if(set_transformation_filter(parameter) == 0)
				{
					text_response_BS(SUCCESS, strcat("SUCCESS. Current transformation: ", parameter), buffer, fd);
				}
				else
				{
					//ERROR Interno
					//TODO: Manejar errores internos y solicitar un nuevo request.
				}
			}
			break;
		case QUIT:
			s->protocol_state = CLOSE;
			//Es multiestado entonces no verifico
			text_response_BS(SUCCESS, "Goodbye!", buffer, fd);
			//cerrar sesion cerrando el socket y dropeando la informacion de sesion
			break;
			//Y por ultimo tenemos el caso default por si falla por scope o por error
		default:
			//El command ingresado no esta dentro de los disponibles.
			text_response_BS(FAILED, "Command unknown. Refer to the RFC.", buffer, fd);
			break;
	}
}

int parse_admin_command(const char *resp)
{
	if(resp == NULL)
	{
		return -1;
	}
	if(strcmp(resp, "USER") == 0)
	{
		return 0;
	}
	if(strcmp(resp, "PASS") == 0)
	{
		return 1;
	}
	if(strcmp(resp, "LISTS") == 0)
	{
		return 2;
	}
	if(strcmp(resp, "STATS") == 0)
	{
		return 3;
	}
	if(strcmp(resp, "ACTIVE") == 0)
	{
		return 4;
	}
	if(strcmp(resp, "FILTER") == 0)
	{
		return 5;
	}
	if(strcmp(resp, "QUIT") == 0)
	{
		return 6;
	}
	return -1;
}


int authenticate(char *user, char *pass)
{
	//Usando las variable globales tiene que authenticate y luego proporcionar el codigo que resulto
	char    *users[2]  = {"pablito", "juancito"};
	char    *passes[2] = {"pabs", "juans"};
	for(int i          = 0; i < 2; i++)
	{
		if(strcmp(users[i], user) == 0)
		{
			if(strcmp(passes[i], pass) == 0)
			{
				return 0;
			}
			return 1;
		}
	}
	return 1;
}

//Retorna -1 si falla
int parse_positive_int(char *string)
{
	char *remain;
	long ret = strtol(string, &remain, 10);
	if(*remain != '\0')
	{
		//Hubo un error
		return -1;
	}
	return (int) ret;
}

//Esta funcion tiene que estar hecha en el proxy exclusivamente y tiene que cargar
//el array de monitor con strings que digan su numero espacio funcion ej: "1 usuariosOnline"
//Devuelve el numero de funciones que hay
char **get_monitor_array()
{
	app_context_p app = get_app_context();
	return app->monitor;
}

//Busca la estadistica correspondiente al monitor(i)
int monitor(int numero)
{
	//Tiene que buscar las metricas en el contexto
	return 0;
}

//Busca en el contexto del proxy el estado actual de la transformacion. ACTIVO o INACTIVO
int get_transformation_state()
{

	return 0;
}

//Carga el estado de transformacion y devuelve 0 si funciono y -1 sino.
int set_transformation_state(int estado)
{

	return 0;
}

//Busca el command actual en el contexto del proxy
char *get_transformation_filter()
{

	return "";
}

//Carga el filtro que le pasan retorna 0 si funciono y -1 sino
int set_transformation_filter(char *filtro)
{

	return 0;
}

//DEPRECATED
/*
admin_response_p requestBuilder(){
    //Recibo el mensaje de sctp
    //sctp_recvmsg
    //Si devuelve -1 doy error
    //Sino armo el admin_response_p
    if(command == NULL || !(estado == 0 || estado == 1) || tam < 0){
        return NULL;
    }
    admin_response_p res = malloc(RESPONSE_SIZE);
    if(res == NULL) {
        //Mensaje de error
        return NULL;
    }
    res->command = malloc(TAM_command);
    if(res->command == NULL) {
        //Mensaje de error
        return NULL;
    }
    strcpy(res->command, command);
    res->estado = estado;
    res->tamcontent = tam;
    res->content = malloc(tam);
    if(res->content == NULL) {
        //Mensaje de error
        return NULL;
    }
    strcpy(res->content, content);
    return res;
}

admin_response_p responseBuilder(char* command, int estado, int tam, char* content){
    if(command == NULL || !(estado == 0 || estado == 1) || tam < 0){
        return NULL;
    }
    admin_response_p res = malloc(RESPONSE_SIZE);
    if(res == NULL) {
        //Mensaje de error
        return NULL;
    }
    res->command = malloc(TAM_command);
    if(res->command == NULL) {
        //Mensaje de error
        return NULL;
    }
    strcpy(res->command, command);
    res->estado = estado;
    res->tamcontent = tam;
    res->content = malloc(tam);
    if(res->content == NULL) {
        //Mensaje de error
        return NULL;
    }
    strcpy(res->content, content);
    return res;
}

 */