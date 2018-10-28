//
// Created by juangod on 14/10/18.
//

#ifndef PROTOS_TPE_STATE_H
#define PROTOS_TPE_STATE_H

#include <unistd.h>
#include "main.h"
#include "../../Shared/include/buffer.h"
#include <pthread.h>

#define BUFFER_SIZE 50

typedef int state_code;
typedef int error_code;

typedef struct stateStruct * state;

typedef enum {
    NOT_WAITING, WAITING
} execution_state;

struct stateStruct {
    pthread_t tid;
    file_descriptor read_fds[3];
    file_descriptor write_fds[3];
    execution_state exec_state;
    state_code id;
    error_code error;
    int processing_mail;
    int parser_pid;
    execution_state (*on_arrive)(state st, file_descriptor fd, int is_read);
    execution_state (*on_resume)(state st, file_descriptor fd, int is_read);
    state_code (*on_leave)(state st);
    buffer_p buffers[3];
    int protocol_state;
    int data_1;
    int data_2;
    int data_3;
    char* user;
    char* pass;
};


#endif //PROTOS_TPE_STATE_H
