//
// Created by juangod on 08/10/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <bits/socket.h>
#include <errno.h>
#include <inttypes.h>
#include "include/options.h"
#include "../Shared/include/executionValidator.h"
#include "../Shared/include/lib.h"
#include "include/proxyParse.h"

static app_context_p app_context = NULL;

static char *version_number = "SNAPSHOT 1.0.0";

void initialize_app_context()
{
	app_context = malloc(sizeof(app_context_t));
	app_context->censored_media_types  = NULL;
	app_context->pop3_path             = NULL;
	app_context->command_specification = NULL;
	app_context->error_path            = NULL;
	app_context->local_port            = 0;
	app_context->isIPV6                = 0;
	app_context->management_path       = NULL;
	app_context->management_port       = 0;
	app_context->origin_port           = 0;
	app_context->replacement_message   = NULL;
	app_context->address_server_string = NULL;
	app_context->addr                  = NULL;
	app_context->has_to_query_dns      = 0;
	app_context->pipelining            = false;
	app_context->pop3filter_version    = version_number;
	char *monitoreo[5]                 = {"1 - Connected muas", "2 - Connected Admins", "3 - Cantidad de muas historicos", "4 - Bytes totales transferidos",
	                                      "5 - Missing"};
	app_context->transform_status = false;
	for(int i = 0; i < 5; i++)
	{
		int len = strlen(monitoreo[i]);
		(app_context->monitor)[i] = calloc(1, len + 1);
		memcpy((app_context->monitor)[i], monitoreo[i], len);
		(app_context->monitor_values)[i] = 0;
	}
}

void destroy_app_context()
{
	free(app_context->command_specification);
	free(app_context);
}

app_context_p get_app_context()
{
	return app_context;
}

void initialize_options()
{
	option_register("Error Path", 'e', error_specification, "/dev/null",
	                "Specifies the file where stderr is rerouted to. By default /dev/null",
	                error_specification_validation, 1);
	option_register("Pop3 Path", 'l', pop3_direction, "0.0.0.0", "Specifies the route where the Proxy will act.",
	                pop3_direction_validation, 1);
	option_register("Management Path", 'L', management_direction, "127.0.0.1",
	                "Specifies the route where the Management service will act.", management_direction_validation, 1);
	option_register("Replacement Message", 'm', replacement_message, "Replaced Part",
	                "Specifies the message to replace filtered text.", replacement_message_validation, 1);
	option_register("Censored Mediatype", 'M', censored_mediatype, "", "Censored media type list",
	                censored_mediatype_validation, 1);
	option_register("Management port", 'o', management_port, "9090",
	                "Specifies the SCTP port the management server listens to. By default 9090",
	                management_port_validation, 1);
	option_register("Local Port", 'p', local_port, "1110",
	                "Specifies the TCP port for incoming POP3 connections. By default 1110", local_port_validation, 1);
	option_register("Origin Port", 'P', origin_port, "110",
	                "Specifies the TCP port where the POP3 server is found in the origin server. By default 110",
	                origin_port_validation, 1);
	option_register("Command Specification", 't', command_specification, "cat",
	                "Command utilized for external transformations. Compatible with system(3). By default applies no transformations.",
	                command_specification_validation, 1);
}

void pop3_direction(char *arg)
{
    //Receives an direction and uses it to specify the address of the proxy service.
    printf("This is pop3 direction: %s!\n",arg);
	fflush(stdout);
	app_context->pop3_path = my_strdup(arg);
}

void error_specification(char *arg)
{
	//Specifies the file where stderr is rerouted to. By default /dev/null
	printf("This is error specification: %s!\n",arg);
	fflush(stdout);
	app_context->error_path = my_strdup(arg);
}

void management_direction(char *arg)
{
	//Receives an direction and uses it to specify the address of the management service.
	printf("This is management direction: %s!\n",arg);
	fflush(stdout);
	app_context->management_path = my_strdup(arg);
}

void replacement_message(char *arg)
{
	//Specifies the message to replace filtered text
	printf("This is replacement message: %s!\n",arg);
	fflush(stdout);
	app_context->replacement_message = my_strdup(arg);

}

void censored_mediatype(char *arg)
{
	//Receives list of media types and applies a condition for the server to censor them 
	printf("This is censored mediatype: %s!\n",arg);
	fflush(stdout);
	app_context->censored_media_types = my_strdup(arg);

}

void management_port(char *arg)
{
	//Specifies the SCTP port the management server listens to. By default 9090
	printf("This is management port: %s!\n",arg);
	fflush(stdout);
	uint16_t val = string_to_port(arg);
	app_context->management_port = val;
}

void local_port(char *arg)
{
	//Specifies the TCP port for incoming POP3 connections. By default 1110
	printf("This is local port: %s!\n",arg);
	fflush(stdout);
	uint16_t val = string_to_port(arg);
	app_context->local_port = val;
}

void origin_port(char *arg)
{
	//Receives a port and specifies it as the origin server POP3 port
	printf("This is origin port: %s!\n",arg);
	fflush(stdout);
	uint16_t val = string_to_port(arg);
	app_context->origin_port = val;
}

void command_specification(char *arg)
{
	//Receives a command from stdin and applies it to the content that passes through proxy.
	printf("This is command_specification: %s!\n",arg);
	fflush(stdout);
	app_context->command_specification = my_strdup(arg);

}

void version()
{
	//TODO: has to print out version
	printf("%s!\n", version_number);
	fflush(stdout);

}

void help()
{
	printf("SINOPSIS:\n\tpop3filter [ POSIX style options ] origin-server\n\tpop3filter -v\n\tpop3filter -h\n\n");
	printf("OPTIONS:\n");
	option_help();
}

void server_string(char *server_string)
{
	printf("This is server string: %s!\n",server_string);
	fflush(stdout);

	char buf[16];

	if(inet_pton(AF_INET, server_string, buf))
	{
		get_app_context()->has_to_query_dns = false;
		get_app_context()->isIPV6           = false;
		printf("IPv4 Address received.\n");
	}
	else if(inet_pton(AF_INET6, server_string, buf))
	{
		get_app_context()->has_to_query_dns = false;
		get_app_context()->isIPV6           = true;
		printf("IPv6 Address received.\n");
	}
	else
	{
		printf("Origin name requires DNS query.\n");
		get_app_context()->has_to_query_dns = true;
	}
	app_context->address_server_string = server_string;
}

uint16_t string_to_port(char *arg)
{
	char *end = 0;
	errno     = 0;
	intmax_t val = strtoimax(arg, &end, 10);
	if(errno == ERANGE || val < 0 || val > UINT16_MAX || end == arg || *end != '\0')
	{
		exit(1);
	}
	return val;
}
