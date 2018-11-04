//
// Created by juangod on 13/10/18.
//

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "include/state.h"
#include "include/options.h"
#include "include/stateMachine.h"
#include "include/MasterStateMachine.h"
#include "include/list.h"
#include "include/stateSelector.h"
#include "../Shared/include/buffer.h"
#include "include/options.h"
#include "include/proxyCommunication.h"
#include "include/adminControl.h"
#include "include/error.h"
#include "../Shared/include/lib.h"
#include "include/error.h"
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>
#include <ctype.h>


state_machine *sm;

state_machine *initialize_master_machine(file_descriptor MUA_sock, file_descriptor admin_sock)
{
	sm = new_machine();
	sm->states = new_list();

	sm->states_amount  = 0;
	sm->previous_state = NULL;
	sm->next_state     = NULL;

	state connect_client = new_state(CONNECT_CLIENT_STATE, CONNECT_CLIENT_on_arrive, CONNECT_CLIENT_on_resume,
	                                 CONNECT_CLIENT_on_leave);
	connect_client->read_fds[0] = MUA_sock;
	add_state(sm, connect_client);

	state connect_admin = new_state(CONNECT_ADMIN_STATE, CONNECT_ADMIN_on_arrive, CONNECT_ADMIN_on_resume,
	                                CONNECT_ADMIN_on_leave);
	connect_admin->read_fds[0] = admin_sock;
	add_state(sm, connect_admin);

	return sm;
}

execution_state ATTEND_ADMIN_on_arrive(state s, file_descriptor fd, int is_read)
{
	switch(is_read)
	{
		case 1:;
			int ret = buffer_read(fd, s->buffers[0]);
			if(ret == 0 || (ret == -1 && errno == ECONNRESET))
			{
				printf("--------------------------------------------------------\n");
				printf("Administrator disconnected \n");
				printf("--------------------------------------------------------\n");
				disconnect(s);
				return WAITING;
			}
			printf("--------------------------------------------------------\n");
			printf("Read buffer content from ADMIN: \n");
			print_buffer(s->buffers[0]);
			break;
		case 0:
			printf("Processing command!\n");
			if(s->remaining_response == 0)
			{
				process_request(s, fd);
			}
			else if(s->remaining_response == -1)
			{
				s->remaining_response = 0;
			}
			buffer_write(fd, s->buffers[0]);
			if(s->remaining_response != 0)
			{
				int read = buffer_read_string(s->remaining_string + s->remaining_response, s->buffers[0]);
				if(*(s->remaining_string + read + s->remaining_response) != 0)
				{
					s->remaining_response += read;
				}
				else
				{
					s->remaining_response = -1;
					free(s->remaining_string);
				}
			}
			else
			{
				if(s->disconnect)
				{
					disconnect(s);
				}
			}
			printf("--------------------------------------------------------\n");
			printf("Wrote buffer content to ADMIN: \n");

			break;
	}
}

execution_state ATTEND_ADMIN_on_resume(state s, file_descriptor fd, int is_read)
{

}

state_code ATTEND_ADMIN_on_leave(state s)
{
	disconnect(s);
}

execution_state CONNECT_ADMIN_on_arrive(state s, file_descriptor fd, int is_read)
{
	socklen_t sl         = sizeof(s->connection_addrinfo);
	int       accept_ret = accept(s->read_fds[0], (s->connection_addrinfo), &sl);
	if(accept_ret == -1)
	{
		perror("accept()");
		disconnect(s);
		return NOT_WAITING;
	}

	//NEW ADMIN CONNECTED
	log_event(s, '+', "Seconecto");
	get_app_context()->monitor_values[1] += 1;

	state st = new_state(CONNECT_ADMIN_STAGE_TWO_STATE, CONNECT_ADMIN_STAGE_TWO_on_arrive,
	                     CONNECT_ADMIN_STAGE_TWO_on_resume, CONNECT_ADMIN_STAGE_TWO_on_leave);
	st->read_fds[0]  = accept_ret;
	st->write_fds[0] = accept_ret;
	buffer_initialize(&(st->buffers[0]),BUFFER_SIZE);
	add_state(sm, st);

	return NOT_WAITING;
}

execution_state CONNECT_ADMIN_on_resume(state s, file_descriptor fd, int is_read)
{

}

state_code CONNECT_ADMIN_on_leave(state s)
{
}

execution_state CONNECT_ADMIN_STAGE_TWO_on_arrive(state s, file_descriptor fd, int is_read)
{
	buffer_read_string("+OK Greetings Earthlings. Now log in.\r\n",s->buffers[0]);
	buffer_write(fd,s->buffers[0]);

	return NOT_WAITING;
}

execution_state CONNECT_ADMIN_STAGE_TWO_on_resume(state s, file_descriptor fd, int is_read)
{

}

state_code CONNECT_ADMIN_STAGE_TWO_on_leave(state s)
{
	state st = new_state(ATTEND_ADMIN_STATE, ATTEND_ADMIN_on_arrive,
	                     ATTEND_ADMIN_on_resume, ATTEND_ADMIN_on_leave);
	st->read_fds[0]  = s->read_fds[0];
	st->write_fds[0] = s->write_fds[0];
	buffer_initialize(&(st->buffers[0]), BUFFER_SIZE);
	add_state(sm, st);
	remove_state(sm,s);
}

void *query_dns(void *st)
{
	state           s     = (state) st;
	struct addrinfo hints = {
			.ai_family    = AF_UNSPEC,
			.ai_socktype  = SOCK_STREAM,
			.ai_flags     = AI_PASSIVE,
			.ai_protocol  = 0,
			.ai_canonname = NULL,
			.ai_addr      = NULL,
			.ai_next      = NULL,
	};

	if(get_app_context()->first!=NULL)
	{
		freeaddrinfo(get_app_context()->first);
	}

	char buff[7];
	snprintf(buff, sizeof(buff), "%hu", get_app_context()->origin_port);
	getaddrinfo(get_app_context()->address_server_string, buff, &hints, &(get_app_context()->first));

	get_app_context()->addr = get_app_context()->first;
	int ret = -1;

	while(get_app_context()->addr != NULL && (ret = connect(s->read_fds[1], get_app_context()->addr->ai_addr, get_app_context()->addr->ai_addrlen)) < 0)
	{
		get_app_context()->addr = get_app_context()->addr->ai_next;
	}


	char buff1[1] = {0};
	char buff2[1] = {1};
	if(ret < 0)
	{
		write(((int *) s->pipes)[1], buff1, 1);
	}
	else
	{
		write(((int *) s->pipes)[1], buff2, 1);
		char buf[10];
		if(inet_pton(AF_INET6, get_app_context()->addr->ai_addr->sa_data, buf))
		{
			printf("pls9\n");
			printf("DNS found IPV6 address.\n");
			get_app_context()->isIPV6 = true;
		}
		else
		{
			printf("pls9\n");
			printf("DNS found IPV4 address.\n");
			get_app_context()->isIPV6 = false;
		}
	}
}

execution_state CONNECT_CLIENT_on_arrive(state s, file_descriptor fd, int is_read)
{
	socklen_t addrlen    = sizeof(s->connection_addrinfo);
	int       accept_ret = accept(s->read_fds[0], (s->connection_addrinfo), &addrlen);

	if(accept_ret < 0)
	{
		perror("accept");
		disconnect_client(s);
		return NOT_WAITING;
	}

	if(get_app_context()->has_to_query_dns)
	{
		state st = new_state(CONNECT_CLIENT_STAGE_TWO_STATE, CONNECT_CLIENT_STAGE_TWO_on_arrive,
		                     CONNECT_CLIENT_STAGE_TWO_on_resume, CONNECT_CLIENT_STAGE_TWO_on_leave);
		st->read_fds[0] = accept_ret;
		st->read_fds[1] = setup_origin_socket();

		int ret = pipe(st->pipes);
		if(ret < 0)
		{
			perror("pipe error on dns query");
			disconnect_client(s);
			return NOT_WAITING;
		}

		pthread_create(&(st->tid), NULL, query_dns, (void *) st);
		st->read_fds[2] = st->pipes[0];

		add_state(sm, st);
	}
	else
	{
		state st = new_state(CONNECT_CLIENT_STAGE_THREE_STATE, CONNECT_CLIENT_STAGE_THREE_on_arrive,
		                     CONNECT_CLIENT_STAGE_THREE_on_resume, CONNECT_CLIENT_STAGE_THREE_on_leave);
		st->read_fds[0] = accept_ret;
		st->read_fds[1] = setup_origin_socket();

		add_state(sm, st);
	}
	return NOT_WAITING;
}

execution_state CONNECT_CLIENT_on_resume(state s, file_descriptor fd, int is_read)
{
}

state_code CONNECT_CLIENT_on_leave(state s)
{
}

execution_state CONNECT_CLIENT_STAGE_THREE_on_arrive(state s, file_descriptor fd, int is_read)
{
	if(get_app_context()->has_to_query_dns)
	{
		// Connection happens in thread
	}
	else
	{
		if(get_app_context()->isIPV6)
		{
			struct sockaddr_in6 address;
			memset(&address, 0, sizeof(address));
			address.sin6_family = AF_INET6;
			address.sin6_port   = htons((uint16_t) get_app_context()->origin_port);
			inet_pton(AF_INET6, get_app_context()->address_server_string, &address.sin6_addr);

			if(connect(s->read_fds[1], (struct sockaddr *) &address, sizeof(address)) < 0)
			{
				perror("Connect to origin error");
				disconnect(s);
				return WAITING;
			}
		}
		else
		{
			struct sockaddr_in address;
			memset(&address, 0, sizeof(address));
			address.sin_port        = htons((uint16_t) get_app_context()->origin_port);
			address.sin_family      = AF_INET;
			address.sin_addr.s_addr = inet_addr(get_app_context()->address_server_string);

			if(connect(s->read_fds[1], (struct sockaddr *) &address, sizeof(address)) < 0)
			{
				perror("Connect to origin error");
				disconnect(s);
				return WAITING;
			}
		}
	}

	state st = new_state(CONNECT_CLIENT_STAGE_FOUR_STATE, CONNECT_CLIENT_STAGE_FOUR_on_arrive,
	                     CONNECT_CLIENT_STAGE_FOUR_on_resume, CONNECT_CLIENT_STAGE_FOUR_on_leave);
	st->read_fds[0]  = s->read_fds[0];
	st->write_fds[0] = s->read_fds[0];
	st->read_fds[1]  = fd;
	st->write_fds[1] = fd;
	int ret[2];
	st->parser_pid = start_parser(get_app_context()->command_specification, ret, s);
	st->read_fds[2]  = ret[0];
	st->write_fds[2] = ret[1];
	st->data_1 = false;
	buffer_initialize(&(st->buffers[0]), BUFFER_SIZE);

	add_state(sm, st);

	//NEW MUA CONNECTED
	get_app_context()->monitor_values[0] += 1;
	get_app_context()->monitor_values[2] += 1;

	return NOT_WAITING;
}

execution_state CONNECT_CLIENT_STAGE_THREE_on_resume(state s, file_descriptor fd, int is_read)
{

}

state_code CONNECT_CLIENT_STAGE_THREE_on_leave(state s)
{
	remove_state(sm, s);
}

execution_state CONNECT_CLIENT_STAGE_FOUR_on_arrive(state s, file_descriptor fd, int is_read)
{
	//      Uses data_1 to store internal state of STAGE_FOUR
	//    ****************************************************
	//    ***   data_1 = 0  ==>   Read +OK from Origin     ***
	//    ***   data_1 = 1  ==>   Write +OK to MUA         ***
	//    ***   data_1 = 2  ==>   Write CAPA to Origin     ***
	//    ***   data_1 = 3  ==>   Process CAPA reply       ***
	//    ****************************************************

	s->data_1     = 0;
	s->exec_state = WAITING;
	return WAITING;
}

execution_state CONNECT_CLIENT_STAGE_FOUR_on_resume(state s, file_descriptor fd, int is_read)
{
	//
	// It is assumed that each line received from CAPA
	// fits in a single buffer (50 octets).
	//
	// It is assumed that the content after the +OK greeting
	// fits in a single buffer (50 octets).
	//
	switch(s->data_1)
	{
		case 0: // Read +OK from Origin
			buffer_read(fd, s->buffers[0]);
			s->data_1 = 1;
			return WAITING;
		case 1: // Write +OK to MUA
			buffer_write(fd, s->buffers[0]);
			s->data_1 = 2;
			return WAITING;
		case 2: // Write CAPA to Origin
			buffer_read_string("CAPA\n", s->buffers[0]);
			buffer_write(fd, s->buffers[0]);
			s->data_1                     = 3;
			get_app_context()->pipelining = false;
			return WAITING;
		case 3: // Process CAPA reply
			buffer_read_until_string(fd, s->buffers[0], "\r\n");
			char buf[BUFFER_SIZE]         = {0};
			buffer_write_string(buf, s->buffers[0]);
			buffer_clean(s->buffers[0]);

			int i = 0;
			while(i < BUFFER_SIZE && buf[i] != 0)
			{
				buf[i] = tolower((int) buf[i]);
				i++;
			}

			if(!strcmp(buf, "pipelining\r\n"))
			{
				get_app_context()->pipelining = true;
				return WAITING;
			}
			else if(!strcmp(buf, ".\r\n"))
			{
				return NOT_WAITING;
			}
			else
			{
				return WAITING;
			}
	}
}

state_code CONNECT_CLIENT_STAGE_FOUR_on_leave(state s)
{
	state st = new_state(ATTEND_CLIENT_STATE, ATTEND_CLIENT_on_arrive, ATTEND_CLIENT_on_resume, ATTEND_CLIENT_on_leave);
	st->read_fds[0]  = s->read_fds[0];
	st->write_fds[0] = s->write_fds[0];
	st->read_fds[1]  = s->read_fds[1];
	st->write_fds[1] = s->write_fds[1];
	st->read_fds[2]  = s->read_fds[2];
	st->write_fds[2] = s->write_fds[2];
	buffer_initialize(&(st->buffers[0]), BUFFER_SIZE);
	buffer_initialize(&(st->buffers[1]), BUFFER_SIZE);
	buffer_initialize(&(st->buffers[2]), BUFFER_SIZE);

	add_state(sm, st);
	remove_state(sm, s);
}

execution_state CONNECT_CLIENT_STAGE_TWO_on_arrive(state s, file_descriptor fd, int is_read)
{
	pthread_join(s->tid, NULL);

	int buff = false;
	if(read(s->pipes[0], &buff, 1) < 0)
	{
		perror("read error");
		disconnect_client(s);
		return WAITING;
	}
	if(buff)
	{
		printf("Connected to DNS origin host correctly.\n");
	}
	else
	{
		printf("Unable to connect to dns origin host.\n");
		disconnect_client(s);
		return WAITING;
	};

	state st = new_state(CONNECT_CLIENT_STAGE_THREE_STATE, CONNECT_CLIENT_STAGE_THREE_on_arrive,
	                     CONNECT_CLIENT_STAGE_THREE_on_resume, CONNECT_CLIENT_STAGE_THREE_on_leave);
	st->read_fds[0] = s->read_fds[0];
	st->read_fds[1] = s->read_fds[1];

	add_state(sm, st);
	return NOT_WAITING;
}

execution_state CONNECT_CLIENT_STAGE_TWO_on_resume(state s, file_descriptor fd, int is_read)
{
}

state_code CONNECT_CLIENT_STAGE_TWO_on_leave(state s)
{
	remove_state(sm, s);
}

execution_state ATTEND_CLIENT_on_arrive(state s, file_descriptor fd, int is_read)
{
	int disconnection = false;
	switch(is_read)
	{
		case true:
			if(s->read_fds[0] == fd)
			{   // MUA READ
				int read_response = buffer_read_until_char(fd, s->buffers[0], '\n');
				buffer_remove_trailing_spaces(s->buffers[0]);
				if(read_response == 0)
				{
					printf("--------------------------------------------------------\n");
					printf("Client disconnected \n");
					printf("--------------------------------------------------------\n");

					// If EOF received, MUA read and Origin write must be closed
					s->disconnects[0]=true;
					shutdown(s->read_fds[0],SHUT_RD);
					s->read_fds[0]=-1;
					s->disconnects[3]=true;
					shutdown(s->write_fds[1],SHUT_WR);
					s->write_fds[1]=-1;

					return WAITING;
				}
				printf("--------------------------------------------------------\n");
				printf("Read buffer content from MUA: \n");
				print_buffer(s->buffers[0]);
			}
			else if(s->read_fds[1] == fd)
			{   // Origin READ
				int rd = buffer_read_until_string(fd, s->buffers[1], "\r\n");
				if(rd == 0)
				{
					printf("--------------------------------------------------------\n");
					printf("Origin disconnected \n");
					printf("--------------------------------------------------------\n");

					// If EOF received, Origin read and Origin write must be closed
					s->disconnects[2]=true;
					shutdown(s->read_fds[1],SHUT_RD);
					s->read_fds[1]=-1;
					s->disconnects[3]=true;
					shutdown(s->write_fds[1],SHUT_WR);
					s->write_fds[1]=-1;

					if(!IS_PROCESSING){
						disconnect(s);
					}

					return WAITING;
				}
				if(rd < 0)
				{
					printf("--------------------------------------------------------\n");
					perror("Origin error \n");
					printf("--------------------------------------------------------\n");
					switch(errno)
					{
						default:
							buffer_read_string("-ERR Origin Error\n", s->buffers[1]);
							break;
					}
					return NOT_WAITING;
				}
				printf("--------------------------------------------------------\n");
				printf("Read buffer content from Origin: \n");
				print_buffer(s->buffers[1]);
				//Saco los termination octets
				//Miro el buffer count
				if(buffer_starts_with_string("+OK", s->buffers[1]) ||
				   buffer_starts_with_string("-ERR", s->buffers[1]))
				{
					IS_NEW_LINE      = true;// ESTE ES NUEVO
					IS_NEXT_NEW_LINE = true; //EL PROXIMO ES NUEVO
					IS_MULTILINE     = false; //NO ES MULTILINEA
					if(!get_app_context()->pipelining)
					{
						s->pipelining_data = true;
					}
				}
				else if(IS_NEXT_NEW_LINE && buffer_indicates_parsable_message(s->buffers[1]))
				{
					IS_NEW_LINE      = true; //ESTE ES LO QUE ERA EL PROXIMO
					IS_NEXT_NEW_LINE = false;// EL PROXIMO NO ES NUEVO
					IS_TRANS         = true; //ESTE ES TRANS
					IS_MULTILINE     = true; //ES MULTILINEA
				}
				else if(IS_NEXT_NEW_LINE && buffer_indicates_start_of_list(s->buffers[1]))
				{
					IS_NEW_LINE      = true; //ESTE es nuevo
					IS_NEXT_NEW_LINE = false;// EL PROXIMO NO ES NUEVO
					IS_TRANS         = true; //ESTE ES TRANS
					IS_MULTILINE     = true; //ES MULTILINEA

				}
				else if(IS_NEXT_NEW_LINE && buffer_indicates_start_of_capa(s->buffers[1]))
				{
					IS_NEW_LINE      = true; //ESTE es nuevo
					IS_NEXT_NEW_LINE = false;// EL PROXIMO NO ES NUEVO
					IS_TRANS         = true; //ESTE ES TRANS
					IS_MULTILINE     = true; //ES MULTILINEA
					if(!get_app_context()->pipelining)
					{
						buffer_read_string("PIPELINING\r\n", s->buffers[1]);
					}
				}
				else
				{ //continuo impresion de multilinea
					IS_NEW_LINE = false; //ESTE NO ES NUEVO
				}
			}
			else if(s->read_fds[2] == fd)
			{   // Transform READ
				if(buffer_read(fd, s->buffers[2]) == 0)
				{
					IS_PROCESSING = false;
					close(s->read_fds[2]);
					s->read_fds[2]  = -1;
					s->write_fds[2]  = -1;

					int ret = check_parser_exit_status(s->parser_pid);
					if(ret == STANDARD)
					{
						printf("OK: Parser exited correctly\n");
					}
					else
					{
						printf("FAIL: BAD PARSER EXIT\n");
					}
					s->parser_pid=-1;
					if(s->disconnects[2]){
						disconnect(s);
						disconnection=true;
					}
				}
				else{
					printf("--------------------------------------------------------\n");
					printf("Read buffer content from Transform: \n");
					print_buffer(s->buffers[2]);
				}
			}
			break;
		case false:
			if(s->write_fds[0] == fd)
			{   // MUA WRITE
				printf("--------------------------------------------------------\n");
				printf("Wrote buffer content to MUA: \n");
				print_buffer(s->buffers[2]);
				int written_size=0;
				int expected_size=0;
				if(!IS_ALREADY_LINE_BUFFERED && buffer_must_be_line_buffered(s->buffers[2]))
				{
					written_size=write(s->write_fds[0],".",1);
					IS_ALREADY_LINE_BUFFERED=true;
					expected_size=1;
				}
				else
				{
					IS_ALREADY_LINE_BUFFERED=false;
					written_size=buffer_write(fd, s->buffers[2]);
					expected_size=BUFFER_SIZE;
				}
				if(written_size<0)
				{
					disconnect(s);
					disconnection=true;
				}
				get_app_context()->monitor_values[3] += written_size;
			}
			else if(s->write_fds[1] == fd)
			{   // Origin WRITE
				print_buffer(s->buffers[0]);
				buffer_write(fd, s->buffers[0]);
				if(!get_app_context()->pipelining)
				{
					s->pipelining_data = false;
				}
				printf("--------------------------------------------------------\n");
				printf("Wrote buffer content to Origin: \n");
			}
			else if(s->write_fds[2] == fd)
			{   // Transform WRITE
				printf("--------------------------------------------------------\n");
				printf("Wrote buffer content to Transform: \n");
				print_buffer(s->buffers[1]);
				int will_close;
				if(IS_MULTILINE)
				{
					will_close = buffer_indicates_end_of_multiline_message(s->buffers[1]);
					if(buffer_is_line_buffered(s->buffers[1]))
					{
						buffer_write_after_index(fd, s->buffers[1], 1);
					}
					else
					{
						buffer_write(fd,s->buffers[1]);
					}
				}
				else
				{
					will_close = buffer_indicates_end_of_single_line_message(s->buffers[1]);
					buffer_write(fd, s->buffers[1]);

				}
				if(will_close)
				{
					close(s->write_fds[2]);
					if(!get_app_context()->pipelining)
					{
						s->pipelining_data = true;
					}
					s->write_fds[2]=-1;
				}
			}
			break;
	}
	if(!disconnection && IS_NEW_LINE && !IS_PROCESSING) //CREATE TRANSFORM
	{
		char *command = (s->data_3 && get_app_context()->transform_status) ? get_app_context()->command_specification
		                                                                   : "cat";
		int  pipes[2];
		s->parser_pid = start_parser(command, pipes, s);
		s->read_fds[2]  = pipes[0];
		s->write_fds[2] = pipes[1];
		IS_PROCESSING = true;
		IS_TRANS      = false;
		IS_NEW_LINE   = false;
		printf("Created new Transform Process with command %s.\n", command);
	}
	return WAITING;
}

execution_state ATTEND_CLIENT_on_resume(state s, file_descriptor fd, int is_read)
{
	ATTEND_CLIENT_on_arrive(s, fd, is_read);
}

state_code ATTEND_CLIENT_on_leave(state s)
{
	disconnect_client(s);
}

void set_up_fd_sets_rec(fd_set *read_fds, fd_set *write_fds, node curr)
{
	if(curr == NULL)
	{
		return;
	}

	switch(curr->st->id)
	{
		case ATTEND_CLIENT_STATE:
			if(curr->st->buffers[0] != NULL)
			{
				if(buffer_is_empty(curr->st->buffers[0]))
				{
					if(curr->st->read_fds[0] > 0)
					{
						if(MUA_READ_DISCONNECTED)
						{
							printf("Origin Write disconnected, fd not added.\n");
						}
						else
						{
							printf("(MUA READ) Buffer 1 is empty ==> ");
							add_read_fd(curr->st->read_fds[0]); // MUA read
						}
					}
				}
				else
				{
					if(curr->st->write_fds[1] > 0)
					{
						printf("(ORIGIN WRITE) Buffer 1 is not empty ==> ");
						if(ORIGIN_WRITE_DISCONNECTED)
						{
							printf("Origin Write disconnected, fd not added.\n");
						}
						else
						{
							if(get_app_context()->pipelining)
							{
								printf("OS has Pipelining enabled. Added origin write\n");
								add_write_fd(curr->st->write_fds[1]); // Origin write
							}
							else
							{
								printf("OS has no Pipelining enabled. ");
								if(curr->st->pipelining_data)
								{
									printf("Added origin write\n");
									add_write_fd(curr->st->write_fds[1]); // Origin write
								}
								else
								{
									printf("Waiting for server to process\n");
								}
							}
						}
					}
				}
			}
			if(curr->st->buffers[1] != NULL)
			{
				if(buffer_is_empty(curr->st->buffers[1]))
				{
					if(ORIGIN_READ_DISCONNECTED){
						printf("Origin read disconnected, no fd added.");
					}
					else{
						if(curr->st->read_fds[1] > 0)
						{
							printf("(ORIGIN READ) Buffer 2 is empty ==> ");
							add_read_fd(curr->st->read_fds[1]);
							// ORIGIN read
						}
					}

				}
				else
				{
					if(curr->st->write_fds[2] > 0)
					{
						printf("(TRANSFORM WRITE) Buffer 2 is not empty ==> ");
						if(curr->st->data_1 && !curr->st->data_4)
						{
							printf("Will write to transform process.\n");
							add_write_fd(curr->st->write_fds[2]); // Transform write
						}
						else
						{
							printf("Can't write, waiting for new transform process.\n");
						}
					}
				}
			}
			if(curr->st->buffers[2] != NULL)
			{
				if(buffer_is_empty(curr->st->buffers[2]))
				{
					if(curr->st->read_fds[2] > 0)
					{
						printf("(TRANSFORM READ) Buffer 3 is empty ==> ");
						add_read_fd(curr->st->read_fds[2]); // Transform read
					}
				}
				else
				{
					if(MUA_WRITE_DISCONNECTED){
						printf("MUA write disconnected, fd not added.");
					}
					else{
						if(curr->st->write_fds[0] > 0)
						{
							printf("(MUA WRITE) Buffer 3 is not empty ==> ");
							add_write_fd(curr->st->write_fds[0]); // MUA write
						}
					}
				}
			}
			break;
		case CONNECT_CLIENT_STATE:
			add_read_fd(curr->st->read_fds[0]);
			break;
		case CONNECT_CLIENT_STAGE_TWO_STATE:
			add_read_fd(curr->st->read_fds[2]);
			break;
		case CONNECT_CLIENT_STAGE_THREE_STATE:
			add_read_fd(curr->st->read_fds[1]);
			break;
		case CONNECT_CLIENT_STAGE_FOUR_STATE:
			switch(curr->st->data_1)
			{
				case 0: // read +OK from Origin
					add_read_fd(curr->st->read_fds[1]);
					break;
				case 1: // send +OK to MUA
					add_write_fd(curr->st->write_fds[0]);
					break;
				case 2: // send CAPA to Origin
					add_write_fd(curr->st->write_fds[1]);
					break;
				case 3: // process CAPA reply
					add_read_fd(curr->st->read_fds[1]);
					break;
			}
			break;
		case ATTEND_ADMIN_STATE:
			if(buffer_is_empty(curr->st->buffers[0]))
			{
				printf("Admin buffer is empty ==> ");
				add_read_fd(curr->st->read_fds[0]);
			}
			else
			{
				printf("Admin buffer is not empty ==> ");
				add_write_fd(curr->st->write_fds[0]);
			}
			break;
		case CONNECT_ADMIN_STAGE_TWO_STATE:
			add_write_fd(curr->st->write_fds[0]);
			break;
		default:;
			int i;
			for(i = 0; i < 3; i++)
			{
				if(curr->st->read_fds[i] != -1 && curr->st->read_fds[i] != -2)
				{
					add_read_fd(curr->st->read_fds[i]);
				}
			}
			for(i = 0; i < 3; i++)
			{
				if(curr->st->write_fds[i] != -1 && curr->st->write_fds[i] != -2)
				{
					add_write_fd(curr->st->write_fds[i]);
				}
			}
			break;
	}

	set_up_fd_sets_rec(read_fds, write_fds, curr->next);
}

void set_up_fd_sets(fd_set *read_fds, fd_set *write_fds)
{
	FD_ZERO(read_fds);
	FD_ZERO(write_fds);
	if(sm == NULL)
	{
		printf("Found null\n");
		fflush(stdout);
	}
	if(sm->states == NULL)
	{
		printf("Found null2\n");
		fflush(stdout);
	}
	set_up_fd_sets_rec(read_fds, write_fds, sm->states->head);
}

void debug_print_state(int state)
{
	char *msg;
	switch(state)
	{
		case CONNECT_CLIENT_STATE:
			msg = "CONNECT_CLIENT_STATE";
			break;
		case CONNECT_CLIENT_STAGE_TWO_STATE:
			msg = "CONNECT_CLIENT_STAGE_TWO_STATE";
			break;
		case CONNECT_CLIENT_STAGE_THREE_STATE:
			msg = "CONNECT_CLIENT_STAGE_THREE_STATE";
			break;
		case CONNECT_CLIENT_STAGE_FOUR_STATE:
			msg = "CONNECT_CLIENT_STAGE_FOUR_STATE";
			break;
		case ATTEND_ADMIN_STATE:
			msg = "ATTEND_ADMIN_STATE";
			break;
		case CONNECT_ADMIN_STATE:
			msg = "CONNECT_ADMIN_STATE";
			break;
		case CONNECT_ADMIN_STAGE_TWO_STATE:
			msg = "CONNECT_ADMIN_STAGE_TWO_STATE";
			break;
		case ATTEND_CLIENT_STATE:
			msg = "ATTEND_CLIENT_STATE";
			break;
		default:
			msg = "State not found in debug print";
			break;
	}
	printf("State %s was chosen.\n", msg);
	fflush(stdout);
}

void disconnect(state st)
{
	shutdown(st->read_fds[0], SHUT_RDWR);
	shutdown(st->write_fds[0], SHUT_RDWR);
	shutdown(st->read_fds[1], SHUT_RDWR);
	shutdown(st->write_fds[1], SHUT_RDWR);
	close(st->write_fds[2]);

	if(st->parser_pid != -1)
	{
		check_parser_exit_status(st->parser_pid);
	}

	close(st->read_fds[2]);

	st->read_fds[0]=-1;
	st->read_fds[1]=-1;
	st->read_fds[2]=-1;
	st->write_fds[0]=-1;
	st->write_fds[1]=-1;
	st->write_fds[2]=-1;

	switch(st->id)
	{
		case CONNECT_CLIENT_STAGE_THREE_STATE:
			get_app_context()->monitor_values[0]--;
			break;
		case CONNECT_CLIENT_STAGE_FOUR_STATE:
			get_app_context()->monitor_values[0]--;
			break;
		case ATTEND_CLIENT_STATE:
			get_app_context()->monitor_values[0]--;
			break;
		case CONNECT_ADMIN_STATE:
			get_app_context()->monitor_values[1]--;
			break;
		case ATTEND_ADMIN_STATE:
			get_app_context()->monitor_values[1]--;
			break;
	}
	remove_state(sm, st);
}

void disconnect_all_rec(state_machine *sm, node curr)
{
	if(curr == NULL)
	{
		return;
	}

	disconnect_all_rec(sm, curr->next);

	disconnect(curr->st);
}

void disconnect_all(state_machine *sm)
{
	disconnect_all_rec(sm, sm->states->head);
}

void log_event(state s, char event, char *data)
{
	//date_time_string La fecha y hora del evento del protocolo. El valor tiene la forma aaaa-mm-ddhh:mm:ss.fffZ, en donde aaaa = año, mm = mes, dd = día, hh = hora, mm = minuto, ss = segundo,fff = fracciones de segundo y Z significa Zulú. Zulú es otra forma de indicar la Hora universal coordinada (UTC).
	//session-id        Un GUID que identifique de manera única la sesión de SMTP asociada con un evento de protocolo.
	//sequence-number (es log_sequence en app_context)   Contador que se inicia en 0 y que aumenta para cada evento dentro de la misma sesión.
	//local-endpoint    El extremo local de una sesión de POP3 o IMAP4. Se compone de una dirección IP y número de puerto TCP, con el formato siguiente: <dirección IP>:<puerto>.
	//remote-endpoint   El extremo remoto de una sesión de POP3 o IMAP4. Se compone de una dirección IP y número de puerto TCP, con el formato siguiente: <dirección IP>:<puerto>.
	//evento            Un único carácter que representa el evento del protocolo. Los valores posibles para el evento son los siguientes: +Conectar -Desconectar >Enviar <Recibir \*Información
	//datos             Información de texto asociada al evento de POP3 o IMAP4.

	app_context_p app_context          = get_app_context();
	//Build date_time_string, no utilizamos fff porque no tenemos esa precision.
	char          date_time_string[21] = {0};
	time_t        now                  = time(NULL);
	struct tm     *t                   = localtime(&now);
	strftime(date_time_string, sizeof(date_time_string) - 1, "%Y-%m-%d%H:%M:%S", t);
	strcat(date_time_string, ".Z");

	char *socket_address = NULL;
	if(s->connection_addrinfo->sa_family == AF_INET)
	{
		struct sockaddr_in *addr_in = (struct sockaddr_in *) s->connection_addrinfo;
		socket_address = malloc(INET_ADDRSTRLEN);
		inet_ntop(AF_INET, &(addr_in->sin_addr), socket_address, INET_ADDRSTRLEN);
	}
	else
	{
		struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *) s->connection_addrinfo;
		socket_address = malloc(INET6_ADDRSTRLEN);
		inet_ntop(AF_INET6, &(addr_in6->sin6_addr), socket_address, INET6_ADDRSTRLEN);
	}

	char aux_port[6]     = {0};
	//Build local-endpoint string
	char *local_endpoint = calloc(1, 2);
	*local_endpoint = '<';
	int old_len      = strlen(local_endpoint);
	int increase_len = strlen(socket_address) + 4;//1 por el > otro pot el : otro el 0 y otro por el <
	local_endpoint = realloc(local_endpoint, old_len + increase_len);
	memset(local_endpoint + old_len, '\0', increase_len);
	strcat(local_endpoint, socket_address);
	strcat(local_endpoint, ">:<");
	old_len = strlen(local_endpoint);
	sprintf(aux_port, "%hu", app_context->local_port);
	increase_len   = strlen(aux_port) + 2;// 1 por > y otro por 0
	local_endpoint = realloc(local_endpoint, old_len + increase_len);
	memset(local_endpoint + old_len, '\0', increase_len);
	strcat(local_endpoint, aux_port);
	strcat(local_endpoint, ">");
	memset(aux_port, '\0', 6);

	//Build remote-endpoint string
	char *remote_endpoint = calloc(1, 2);
	*remote_endpoint = '<';
	old_len         = strlen(remote_endpoint);
	increase_len    = strlen(app_context->pop3_path) + 4;//1 por el > otro pot el : otro el 0 y otro por el <
	remote_endpoint = realloc(remote_endpoint, old_len + increase_len);
	memset(remote_endpoint + old_len, '\0', increase_len);
	strcat(remote_endpoint, app_context->pop3_path);
	strcat(remote_endpoint, ">:<");
	old_len = strlen(remote_endpoint);
	sprintf(aux_port, "%hu", app_context->origin_port);
	increase_len    = strlen(aux_port) + 2;// 1 por > y otro por 0
	remote_endpoint = realloc(remote_endpoint, old_len + increase_len);
	memset(remote_endpoint + old_len, '\0', increase_len);
	strcat(remote_endpoint, aux_port);
	strcat(remote_endpoint, ">");

	void    *array[7]        = {date_time_string, NULL, NULL, local_endpoint, remote_endpoint, NULL, data};
	//Aumento el numero de secuencia
	int     sequence_number  = ++(get_app_context()->log_sequence);
	long    session_id       = s->session_id;
	//Armo el string concatenando con ","
	char    *log             = calloc(1, 1);
	char    *aux_number      = calloc(1, 24);
	int     aux_len          = 0;
	int     aux_increase_len = 0;
	for(int i                = 0; i < 7; i++)
	{
		switch(i)
		{
			case 0:
				aux_len          = strlen(log);
				aux_increase_len = strlen(array[i]) + 1;//1 por la coma y otro por el final de string
				log              = realloc(log, aux_len + aux_increase_len);
				memset(log + aux_len, '\0', aux_increase_len);
				strcat(log, array[i]);
				break;
			case 3:
			case 4:
			case 6:
				aux_len          = strlen(log);
				aux_increase_len = strlen(array[i]) + 2;//1 por la coma y otro por el final de string
				log              = realloc(log, aux_len + aux_increase_len);
				memset(log + aux_len, '\0', aux_increase_len);
				strcat(log, ",");
				strcat(log, array[i]);
				break;
			case 1:
				sprintf(aux_number, "%ld", session_id);
				aux_len          = strlen(log);
				aux_increase_len = strlen(aux_number) + 2;
				log              = realloc(log, aux_len + aux_increase_len);
				memset(log + aux_len, '\0', aux_increase_len);
				strcat(log, ",");
				strcat(log, aux_number);
				memset(aux_number, '\0', 24);
				break;
			case 2:
				sprintf(aux_number, "%d", sequence_number);
				aux_len          = strlen(log);
				aux_increase_len = strlen(aux_number) + 2;
				log              = realloc(log, aux_len + aux_increase_len);
				memset(log + aux_len, '\0', aux_increase_len);
				strcat(log, ",");
				strcat(log, aux_number);
				memset(aux_number, '\0', 24);
				break;
			case 5:
				aux_len          = strlen(log);
				aux_increase_len = 3;//1 por el char, 1 por la coma y otro por el final del string
				log              = realloc(log, aux_len + aux_increase_len);
				memset(log + aux_len, '\0', aux_increase_len);
				strcat(log, ",");
				*(log + (size_t) strlen(log)) = event;
				break;
		}
	}
	printf("%s\n", log);
	free(socket_address);
	free(local_endpoint);
	free(remote_endpoint);
	free(aux_number);
	free(log);
}