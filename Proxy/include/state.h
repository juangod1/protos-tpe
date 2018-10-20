//
// Created by juangod on 14/10/18.
//

#ifndef PROTOS_TPE_STATE_H
#define PROTOS_TPE_STATE_H

#include <unistd.h>
#include "main.h"

typedef int state_code;
typedef int error_code;

typedef struct stateStruct * state;

typedef enum {
    NOT_WAITING, WAITING
} execution_state;

struct stateStruct {
    file_descriptor read_fds[3];
    file_descriptor write_fds[3];
    execution_state exec_state;
    state_code id;
    error_code error;
    execution_state (*on_arrive)(state st, file_descriptor fd);
    execution_state (*on_resume)(state st, file_descriptor fd);
    state_code (*on_leave)();
};

#endif //PROTOS_TPE_STATE_H
