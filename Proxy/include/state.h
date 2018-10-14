//
// Created by juangod on 14/10/18.
//

#ifndef PROTOS_TPE_STATE_H
#define PROTOS_TPE_STATE_H

#include <unistd.h>
#include "main.h"

typedef int state_code;
typedef int error_code;

typedef enum {
    NOT_WAITING, WAITING_READ, WAITING_WRITE
} execution_state;

struct stateStruct {
    file_descriptor wait_read_fd;
    file_descriptor wait_write_fd;
    execution_state exec_state;
    state_code internal_state;
    state_code id;
    error_code error;
    execution_state (*on_arrive)();
    execution_state (*on_resume)();
    state_code (*on_leave)();
};

typedef struct stateStruct * state;

#endif //PROTOS_TPE_STATE_H
