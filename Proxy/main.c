#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/param.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <fcntl.h>
#include "include/proxyCommunication.h"
#include "include/main.h"
#include "include/error.h"
#include "../Shared/include/executionValidator.h"
#include "../Shared/include/lib.h"
#include "include/proxyParse.h"
#include "include/stateMachine.h"
#include "include/MasterStateMachine.h"
#include "include/stateSelector.h"
#include "../Shared/include/buffer.h"
#include "include/mediaTypes.h"
#include "include/options.h"
#include "include/adminControl.h"
#include "include/error.h"


static app_context_p app_context;

int main(int argc, char **argv)
{
	initialize_app_context();
	initialize_options();

	int response = proxy_parse(argc, argv);

	switch(response)
	{
		case STANDARD:

			execute_options();
			server_string(argv[argc - 1]);
			app_context = get_app_context();
			/*
				int pipes[2];
				int pid= start_parser(app_context->command_specification,pipes);

				buffer_p buffer;
				buffer_initialize(&buffer,1000);

				buffer_read(pipes[READ_FD],buffer);

				buffer_write(STDOUT_FILENO,buffer);

				int response = check_parser_exit_status(pid);
				if(response==STANDARD)
				{
					printf("Program exited normally.\n");
				}
				else
				{
					printf("Program encountered an error.\n");
				}

				buffer_finalize(buffer);*/

			//createConection();
			run_server();
			break;
		case HELP:
			help();
			break;
		case VERSION:
			version();
			break;
		default:
			printf("Program execution stopped.\n");
			break;
	}

	destroy_app_context();
	return response;
}

void run_server()
{
	file_descriptor mua      = setup_MUA_socket();
	file_descriptor admin    = setup_admin_socket();
	state_machine   *machine = initialize_master_machine(mua, admin);
	initialize_selector();
	init_error(machine);

	for(;;)
	{
		run_state(machine);
	}
}

file_descriptor setup_origin_socket()
{
	if(get_app_context()->isIPV6)
	{
		file_descriptor sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if(sock < 0)
		{
			perror("Unable to create socket.\n");
			error_terminal();
			return -1;
		}

		return sock;
	}
	else
	{
		file_descriptor sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sock < 0)
		{
			perror("Unable to create socket.\n");
			error_terminal();
			return -1;
		}

		return sock;
	}

}

file_descriptor setup_MUA_socket()
{
	if(get_app_context()->isIPV6)
	{
		struct sockaddr_in6 address;
		memset(&address, 0, sizeof(address));
		address.sin6_port   = htons(app_context->local_port);
		address.sin6_family = AF_INET6;
		address.sin6_addr   = in6addr_any;

		int             flag = 1;
		file_descriptor sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

		if(sock < 0)
		{
			perror("Unable to create socket.");
			error_terminal();
			return -1;
		}

		if(bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
		{
			perror("Unable to bind socket.\n");
			error_terminal();
			return -1;
		}

		if(listen(sock, MAXIMUM_PENDING_CONNECTIONS) < 0)
		{
			perror("Unable to listen.\n");
			error_terminal();
			return -1;
		}

		return sock;
	}
	else
	{
		struct sockaddr_in address;
		memset(&address, 0, sizeof(address));
		address.sin_port        = htons(app_context->local_port);
		address.sin_family      = AF_INET;
		address.sin_addr.s_addr = htonl(INADDR_ANY);

		int             flag = 1;
		file_descriptor sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

		if(sock < 0)
		{
			perror("Unable to create socket.");
			error_terminal();
			return -1;
		}

		if(bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
		{
			perror("Unable to bind socket.\n");
			error_terminal();
			return -1;
		}

		if(listen(sock, MAXIMUM_PENDING_CONNECTIONS) < 0)
		{
			perror("Unable to listen.\n");
			error_terminal();
			return -1;
		}

		return sock;
	}
}

int findMax(int *a, int size)
{
	int i, m = 0;
	for(i = 0; i < size; i++)
	{
		m = max(m, a[i]);
	}
	return m;
}

int max(int a, int b)
{
	return a > b ? a : b;
}
