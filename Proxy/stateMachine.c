//
// Created by juangod on 13/10/18.
//
#include <malloc.h>
#include "include/stateMachine.h"
#include "include/stateSelector.h"

state * new_state(state_code id, error_code error, execution_state (*onArrive)(), execution_state (*onResume)(), state_code (*onLeave)()){
    state * new = malloc(sizeof(state));
    new->id = id;
    new->error = error;
    new->onArrive = onArrive;
    new->onResume = onResume;
    new->onLeave = onLeave;
    new->exec_state = NOT_WAITING;
    new->internal_state=0;
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

state_machine * new_machine(int states_amount, state * states, state_code initial_state, state_code next_state){
    state_machine * new = malloc(sizeof(state_machine));
    new->next_state = initial_state;
    new->initial_state = initial_state;
    new->states_amount = states_amount;
    new->states = states;
    return new;
}

void run_state(state_machine * sm)
{
    state_code next = select_state();
    state * st = get_state_by_code(next,sm);

    switch(st->exec_state)
    {
        case NOT_WAITING:
            switch(st->onArrive()){
                case NOT_WAITING:
                    st->onLeave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd);
                    break;
            }
            break;
        case WAITING_READ:
            switch(st->onResume()){
                case NOT_WAITING:
                    st->onLeave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd);
                    break;
            }
            break;
        case WAITING_WRITE:
            switch(st->onResume()){
                case NOT_WAITING:
                    st->onLeave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd);
                    break;
            }
            break;
        default:
            break;
    }
}

state * get_state_by_code(state_code code, state_machine * sm){
    int i;
    for(i=0;i<sm->states_amount;i++){
        if(sm->states[i].id==code)
            return &(sm->states[i]);
    }
    return NULL;
}