//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_STATEMACHINE_H
#define PROTOS_TPE_STATEMACHINE_H

typedef unsigned char state_code;
typedef unsigned char error_code;

typedef enum {
    NOT_WAITING, WAITING
} execution_state;

typedef struct {
    state_code id;
    error_code error;
    execution_state exec_state;
    void (*onArrive)();
    void (*onResume)();
    state_code (*onLeave)();
} state;

typedef struct {
    int states_amount;
    state * states;
    state_code initial_state;
    state_code current_state;
} state_machine;

state * new_state(state_code id, error_code error, execution_state exec_state, void (*onArrive)(), void (*onResume)(), state_code (*onLeave)());
void free_state(state * st);
void free_machine(state_machine * machine);
state_machine * new_machine(int states_amount, state * states, state_code initial_state, state_code current_state);

#endif //PROTOS_TPE_STATEMACHINE_H