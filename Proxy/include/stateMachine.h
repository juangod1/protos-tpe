//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_STATEMACHINE_H
#define PROTOS_TPE_STATEMACHINE_H

#include "main.h"

typedef unsigned char state_code;
typedef unsigned char error_code;

typedef enum {
    NOT_WAITING, WAITING_READ, WAITING_WRITE
} execution_state;

typedef struct {
    file_descriptor wait_read_fd;
    file_descriptor wait_write_fd;
    execution_state exec_state;
    unsigned internal_state;
    state_code id;
    error_code error;
    execution_state (*onArrive)();
    execution_state (*onResume)();
    state_code (*onLeave)();
} state;

typedef struct {
    int states_amount;
    state * states;
    state_code initial_state;
    state_code next_state;
} state_machine;

state * new_state(state_code id, error_code error, execution_state (*onArrive)(), execution_state (*onResume)(), state_code (*onLeave)());
void free_state(state * st);
void free_machine(state_machine * machine);
state_machine * new_machine(int states_amount, state * states, state_code initial_state, state_code next_state);
state * get_state_by_code(state_code code, state_machine * sm);
void run_state(state_machine * sm);

#endif //PROTOS_TPE_STATEMACHINE_H