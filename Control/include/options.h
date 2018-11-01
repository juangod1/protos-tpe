#ifndef PROTOS_TPE_OPTIONSADMIN_H
#define PROTOS_TPE_OPTIONSADMIN_H

#include <stdint.h>

typedef struct
{
	uint16_t port;
	char     *listenaddress;
} admin_context_t;

typedef admin_context_t *admin_context_p;

int parse_arguments(int argc, char **argv);

admin_context_p get_admin_context();

void initialize_options();

void initialize_admin_context();

void listen_address(char *address);

uint16_t connection_port(char *port);

int connection_port_validation(uint16_t port);

int listen_address_validation(char *address);

void free_struct();

#endif //PROTOS_TPE_OPTIONS_H
