//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_STATEMACHINE_H
#define PROTOS_TPE_STATEMACHINE_H

#include "main.h"
#include "list.h"
#include "state.h"

typedef struct {
    list states;
    int states_amount;
    state previous_state;
    state next_state;
} state_machine;

state new_state(state_code id, execution_state (*on_arrive)(), execution_state (*on_resume)(), state_code (*on_leave)());
void free_state(state st);
void free_machine(state_machine * machine);
state_machine * new_machine();
void run_state(state_machine * sm);
state get_state_by_code(state_code code, state_machine * sm);
void add_state(state_machine * sm, state s);
int remove_state(state_machine * sm, state s);
void set_write_fd(state st, file_descriptor fd);
void set_read_fd(state st, file_descriptor fd);

#endif //PROTOS_TPE_STATEMACHINE_H