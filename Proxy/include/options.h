//
// Created by juangod on 08/10/18.
//

#ifndef PROTOS_TPE_OPTIONS_H
#define PROTOS_TPE_OPTIONS_H

#include <stdint.h>
#define MONITORING_OPTIONS 4
typedef struct
{
	int             has_to_query_dns;
	char            *address_server_string;
	struct addrinfo *addr;
	struct addrinfo *first;
	int	            error_descriptor;
	char            *pop3_path;
	char            *management_path;
	char            *replacement_message;
	char            *censored_media_types;
	uint16_t        management_port;
	uint16_t        local_port;
	uint16_t        origin_port;
	char            *command_specification;
	char            *monitor[5];
	long            monitor_values[5];
	char            pipelining;
	int             transform_status;
	char            *pop3filter_version;
	int             isIPV6;
	int				log_sequence;
} app_context_t;

typedef app_context_t *app_context_p;


void initialize_options();

void initialize_app_context();

app_context_p get_app_context();

void destroy_app_context();

void pop3_direction(char *arg);

void error_specification(char *arg);

void management_direction(char *arg);

void replacement_message(char *arg);

void censored_mediatype(char *arg);

void management_port(char *arg);

void local_port(char *arg);

void origin_port(char *arg);

void command_specification(char *arg);

void help();

void version();

void server_string(char *server_string);

uint16_t string_to_port(char *arg);


#endif //PROTOS_TPE_OPTIONS_H
