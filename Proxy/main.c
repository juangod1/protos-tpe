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
#include <strings.h>
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

file_descriptor setup_origin_socket(int isip4)
{
	if(isip4)
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
	else
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

}

file_descriptor setup_MUA_socket()
{
	int listenSock, ret;
	size_t size;
	if(get_app_context()->pop3path_is_ipv4 == -1)
	{
		struct sockaddr_in6 servaddr;
		size = sizeof(servaddr);

		listenSock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if(listenSock == -1)
		{

			perror("socket()");
			error_terminal();
		}

		bzero((void *) &servaddr, sizeof(servaddr));
		servaddr.sin6_family       = AF_INET6;
		servaddr.sin6_addr = in6addr_any;
		servaddr.sin6_port         = htons(get_app_context()->local_port);

		ret = bind(listenSock, (struct sockaddr *) &servaddr, (socklen_t)size);

		if(ret == -1)
		{

			perror("bind()");
			close(listenSock);
			error_terminal();
		}
	}
	else if(get_app_context()->pop3path_is_ipv4)
	{
		struct sockaddr_in servaddr;
		size = sizeof(servaddr);

		listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(listenSock == -1)
		{

			perror("socket()");
			error_terminal();
		}

		bzero((void *) &servaddr, sizeof(servaddr));
		servaddr.sin_family      = AF_INET;
		struct in_addr buf;
		inet_pton(AF_INET, get_app_context()->pop3_path, (void *) &buf);
		servaddr.sin_addr = buf;
		servaddr.sin_port        = htons(get_app_context()->local_port);

		ret = bind(listenSock, (struct sockaddr *)&servaddr, (socklen_t)size);

		if(ret == -1)
		{

			perror("bind()");
			close(listenSock);
			error_terminal();
		}
	}
	else
	{
		struct sockaddr_in6 servaddr;
		size = sizeof(servaddr);

		listenSock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if(listenSock == -1)
		{

			perror("socket()");
			error_terminal();
		}

		bzero((void *) &servaddr, sizeof(servaddr));
		servaddr.sin6_family       = AF_INET6;
		struct in6_addr buf;
		inet_pton(AF_INET6, get_app_context()->pop3_path, (void *) &buf);
		servaddr.sin6_addr = buf;
		servaddr.sin6_port         = htons(get_app_context()->local_port);

		ret = bind(listenSock, (struct sockaddr *) &servaddr, (socklen_t)size);

		if(ret == -1)
		{

			perror("bind()");
			close(listenSock);
			error_terminal();
		}
	}

	ret = listen(listenSock, 5);
	if(ret == -1)
	{

		perror("listen()");
		close(listenSock);
		error_terminal();
	}

	return listenSock;
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
