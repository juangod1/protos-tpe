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
#include "include/MasterStateMachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/sctp.h>

#define MAX_BUFFER 1024
#define MAX_FILTER_BUFFER 256

file_descriptor setup_admin_socket()
{
	int                 listenSock, ret;
	struct sockaddr_in  servaddr;

	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if(listenSock == -1)
	{
		printf("Failed to create socket\n");
		perror("socket()");
		error_terminal();
	}

	bzero((void *) &servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	servaddr.sin_port        = htons(get_app_context()->management_port);

	ret = bind(listenSock, (struct sockaddr *) &servaddr, sizeof(servaddr));

	if(ret == -1)
	{
		printf("Bind failed \n");
		perror("bind()");
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

int text_response_BS(int response_state, char *content, state s, file_descriptor fd)
{
	char *res = calloc(1,300);
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
		strcat(res, esp);
	}
	strcat(res, content);
	strcat(res,"\r\n\r\n");
	int read = buffer_read_string(res,s->buffers[0]);
	if(*(res+read) != 0)
	{
		s->remaining_response = read;
		s->remaining_string = res;
	}
	else
	{
		free(res);
	}
	return 0;

}


int parse_message(const char *str, char sep, char **command, char **parameter)
{
	int          count = 0;
	int     escape_found = 0;
	unsigned int start = 0, stop;
	for(stop = 0; str[stop]; stop++)
	{
		if((str[stop] == sep && escape_found == 0))
		{
			if(count == 1)
			{
				return 1;
			}
			*command = calloc(1, stop - start + 1);
			if(stop-start != 0)
			{
                memcpy(*command, str + start, stop - start);
                count++;
                start = stop + 1;
            } else {
                return 1;
			}
		}
		else if(str[stop] == '@' && escape_found == 0)
		{
			escape_found = 1;
			start++;
		}
	}
	if(count == 0)
	{
		*command = calloc(1, stop - start + 1);
		if(stop-start != 0)
		{
            memcpy(*command, str + start, stop - start);
            count++;
            start = stop + 1;
        }
        else
        {
            return 1;
        }
		return 0;
	}
	if(stop-start != 0)
	{
        *parameter = calloc(1, stop - start + 1);
        memcpy(*parameter, str + start, stop - start);
    }
    else
    {
        return 1;
    }
	return 0;
}

void admin_greeting(state s, file_descriptor fd)
{
    text_response_BS(SUCCESS, "Successfull conection! Now log in.", s, fd);
}

void process_request(state s, file_descriptor fd)
{
	buffer_p buffer               = s->buffers[0];
	char     response[MAX_FILTER_BUFFER] = {0};
	buffer_write_string(response, buffer);
	char*p;
	if((p = strchr(response,'\n')) != NULL){
		*p = '\0';
	}
	printf("%s", response);
	char *command   = NULL;
	char *parameter = NULL;
	if(parse_message(response, ' ', &command, &parameter) == 1)
	{
		text_response_BS(FAILED, "Message format error.", s, fd);
		//TODO:Puedo cerrar la conexion o dejar que siga un poco mas.
	}
	else {
        switch (parse_admin_command(command)) {
            case USER:
                if (s->protocol_state != AUTENTICATION) {
                    SCOPE_ERROR
                    //TODO:Cerrar la response y solicitar un nuevo request.
                    break;
                }
                if (parameter != NULL) {
                    s->user = calloc(1, strlen(parameter) + 1);
                    memcpy(s->user, parameter, strlen(parameter));
                    text_response_BS(SUCCESS, "Continue with PASS.", s, fd);
                } else {
                    text_response_BS(FAILED, "Missing argument", s, fd);
                }
                break;
            case PASS:
                if (s->protocol_state != AUTENTICATION) {
                    SCOPE_ERROR
                    //TODO:Cerrar la response y solicitar un nuevo request.
                    break;
                }
                if (s->user != NULL) {
                    s->pass = calloc(1, strlen(parameter) + 1);
                    memcpy(s->pass, parameter, strlen(parameter));
                    int auth = authenticate(s->user, s->pass);
                    if (auth == 0) {
                        text_response_BS(SUCCESS, "Entering EXCHAGE.", s, fd);
                        s->protocol_state = EXCHANGE;
                    } else if (auth == 1) {
                        text_response_BS(FAILED, "FAILED. Try again or QUIT.", s, fd);
                    } else {
                        text_response_BS(FAILED, "Connection error, try again later.", s, fd);
                    }
                } else {
                    text_response_BS(FAILED, "Missing USER.", s, fd);
                }
                break;
            case LISTS:
                if (s->protocol_state != EXCHANGE) {
                    SCOPE_ERROR
                    //TODO:Cerrar la response y solicitar un nuevo request.
                    break;
                }
                if (parameter != NULL) {
                    FORMAT_ERROR
                    //TODO:Cerrar la response y solicitar un nuevo request.
                } else {
                    char content[300] = {0};
                    strcat(content, "List:");
                    char **monitorArray = get_monitor_array();
                    for (int i = 0; i < MONITORING_OPTIONS; i++) {
                        strcat(content, "\n");
                        strcat(content, monitorArray[i]);
                    }
                    text_response_BS(SUCCESS, content, s, fd);
                }
                break;
            case STATS:
                if (s->protocol_state != EXCHANGE) {
                    SCOPE_ERROR
                    //TODO:Cerrar la response y solicitar un nuevo request.
                } else if (parameter != NULL) {
                    int paramNum = parse_positive_int(parameter);
                    if (paramNum == -1) {
                        FORMAT_ERROR
                        //TODO:Cerrar la response y solicitar un nuevo request.
                    } else {
                        long resmonitor = monitor(paramNum);
                        if (resmonitor == -1) {
                            text_response_BS(FAILED, "Option not found, use LISTS.", s, fd);
                        } else {
                            char textomonitor[25];
                            char* content = calloc(1,64);
                            switch(paramNum)
							{
							    case 1:
							        memcpy(content,"Connected MUAs: ", strlen("Connected MUAs: "));
							        break;
							    case 2:
                                    memcpy(content,"Connected admins: ", strlen("Connected admins: "));
							        break;
							    case 3:
                                    memcpy(content,"Historical MUA accesses: ", strlen("Historical MUA accesses: "));
							        break;
							    case 4:
                                    memcpy(content,"Total transferred bytes: ", strlen("Total transferred bytes: "));
							        break;
                                default:
                                    memcpy(content,"The result is: ", strlen("The results is: "));
                                    break;
							}
                            sprintf(textomonitor, "%ld", resmonitor);
                            text_response_BS(SUCCESS, strcat(content, textomonitor), s,
                                             fd);
                            free(content);
                        }
                    }
                } else {
                    FORMAT_ERROR
                }
                break;
            case ACTIVE:
                if (s->protocol_state != EXCHANGE) {
                    SCOPE_ERROR
                    //TODO:Cerrar la response y solicitar un nuevo request.
                } else {
                    if (parameter == NULL) {
                        //Significa que no paso parameter entonces quiere saber cual es el estado actual
                        int estadoT = get_transformation_state();
                        char resp[MAX_FILTER_BUFFER] = "Transformation is: ";
                        text_response_BS(SUCCESS, strcat(resp, (estadoT ? "Active" : "Inactive")), s, fd);
                    } else {
                        int paramNum = parse_positive_int(parameter);
                        if (paramNum != 1 && paramNum != 0) {
                            FORMAT_ERROR
                            //TODO:Cerrar la response y solicitar un nuevo request.
                        } else {
                            set_transformation_state(paramNum);
                            char resp[40] = "SUCCESS. Transformation is: ";
                            text_response_BS(SUCCESS, strcat(resp,
                                                             (paramNum ? "Active" : "Inactive")), s, fd);
                        }
                    }
                }
                break;
            case FILTER:
                if (s->protocol_state != EXCHANGE) {
                    SCOPE_ERROR
                    //TODO:Cerrar la response y solicitar un nuevo request.
                } else {
                    if (parameter == NULL) {
                        char *filtro = get_transformation_filter();
                        char resp[MAX_FILTER_BUFFER] = "Current transformation: ";
                        text_response_BS(SUCCESS, strcat(resp, filtro), s, fd);
                    } else {
                        command_specification(parameter);
                        char resp[MAX_FILTER_BUFFER] = "SUCCESS. Current transformation: ";
                        text_response_BS(SUCCESS, strcat(resp, parameter), s, fd);
                    }
                }
                break;
            case QUIT:
                s->protocol_state = CLOSE;
                text_response_BS(SUCCESS, "Quitting", s, fd);
                free(s->user);
                free(s->pass);
                s->disconnect = 1;
                break;
            default:
                text_response_BS(FAILED, "Command unknown. Refer to the RFC.", s, fd);
                break;
        }
    }
    free(command);
	free(parameter);
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
	char    *users[2]  = {"juan", "marcelo"};
	char    *passes[2] = {"juan", "marcelo"};
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

char **get_monitor_array()
{
	app_context_p app = get_app_context();
	return app->monitor;
}

long monitor(int number)
{
	//TODO Tiene que buscar las metricas en el contexto
	if(number > MONITORING_OPTIONS || number < 1)
        return -1;
	return get_app_context()->monitor_values[number-1];
}

int get_transformation_state()
{

	return get_app_context()->transform_status;
}

void set_transformation_state(int estado)
{
	get_app_context()->transform_status = estado;
}

char *get_transformation_filter()
{
	return get_app_context()->command_specification;
}
