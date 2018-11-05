//
// Created by juangod on 14/10/18.
//

#ifndef PROTOS_TPE_STATE_H
#define PROTOS_TPE_STATE_H

#include <unistd.h>
#include "main.h"
#include "../../Shared/include/buffer.h"
#include <pthread.h>

#define BUFFER_SIZE 600

#define IS_PROCESSING s->data_1
#define IS_NEXT_NEW_LINE s->data_2
#define IS_TRANS s->data_3
#define IS_NEW_LINE s->data_4
#define IS_MULTILINE s->data_5
#define IS_ALREADY_LINE_BUFFERED s->data_6

typedef int state_code;
typedef int error_code;

typedef struct stateStruct *state;

typedef enum
{
	NOT_WAITING, WAITING
}                          execution_state;

struct stateStruct
{
	int             pipes[2];
	pthread_t       tid;
	file_descriptor read_fds[3];
	file_descriptor write_fds[3];
	execution_state exec_state;
	state_code      id;
	error_code      error;
	int             processing_mail;
	int             parser_pid;

	execution_state (*on_arrive)(state st, file_descriptor fd, int is_read);

	execution_state (*on_resume)(state st, file_descriptor fd, int is_read);

	state_code (*on_leave)(state st);

	buffer_p        buffers[3];
	int             protocol_state;
	int             data_1;
	int             data_2;
	int             data_3;
	int             data_4;
	int             data_5;
	int 			data_6;
	int             pipelining_data;
	char            *user;
	char            *pass;
	struct addrinfo *current;
	int             disconnect;
	long            session_id;
	int             remaining_response;
	char            *remaining_string;
	int disconnects[4];
};


#endif //PROTOS_TPE_STATE_H
