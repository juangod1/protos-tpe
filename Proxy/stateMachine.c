//
// Created by juangod on 13/10/18.
//
#include <malloc.h>
#include "include/stateMachine.h"

state * new_state(state_code id, error_code error, execution_state exec_state, void (*onArrive)(), void (*onResume)(), state_code (*onLeave)()){
    state * new = malloc(sizeof(state));
    new->id = id;
    new->error = error;
    new->exec_state = exec_state;
    new->onArrive = onArrive;
    new->onResume = onResume;
    new->onLeave = onLeave;
    return new;
}

void free_state(state * st){
    free(st);
}

void free_machine(state_machine * machine){
    int i;
    for(i=0;i<machine->states_amount;i++)
    {
        free_state(&machine->states[i]);
    }
}

state_machine * new_machine(int states_amount, state * states, state_code initial_state, state_code current_state){
    state_machine * new = malloc(sizeof(state_machine));
    new->current_state = initial_state;
    new->initial_state = initial_state;
    new->states_amount = states_amount;
    new->states = states;
    return new;
}