//
// Created by juangod on 13/10/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "include/stateMachine.h"
#include "include/stateSelector.h"
#include "include/state.h"

state new_state(state_code id, execution_state (*on_arrive)(state s, file_descriptor fd), execution_state (*on_resume)(state s, file_descriptor fd), state_code (*on_leave)(state s)){
    state new = malloc(sizeof(struct stateStruct));
    new->socket_read_fd=-1;
    new->socket_write_fd=-1;
    new->pipe_read_fd=-1;
    new->pipe_write_fd=-1;
    new->id = id;
    new->error = 0;
    new->on_arrive = on_arrive;
    new->on_resume = on_resume;
    new->on_leave = on_leave;
    new->exec_state = NOT_WAITING;
    return new;
}

void set_write_fd(state st, file_descriptor fd){
    st->socket_write_fd=fd;
}

void set_read_fd(state st, file_descriptor fd){
    st->socket_read_fd=fd;
}

void free_state(state st){
    free(st);
}

void free_machine(state_machine * machine){
    free_list(machine->states);
    free(machine);
}

state_machine * new_machine(){
    state_machine * new = malloc(sizeof(state_machine));
    return new;
}

void run_state(state_machine * sm)
{
    state previous = sm->previous_state;

    if(previous!=NULL&&previous->error){
        // error state has fd -1
        state err = get(sm->states,-1);
        err->on_arrive(err,-1);
        err->on_leave();
    }

    file_descriptor next = select_state();
    state st = get(sm->states,next);
    if(st==NULL){
        perror("Error, no state with file descriptor found.");
    }
    printf("State %d was chosen.",st->id);fflush(stdout);

    switch(st->exec_state)
    {
        case NOT_WAITING:
            switch(st->on_arrive(st, next)){
                case NOT_WAITING:
                    st->on_leave();
                    break;
                case WAITING:
                    break;
            }
            break;
        case WAITING:
            switch(st->on_resume(st, next)){
                case NOT_WAITING:
                    st->on_leave();
                    break;
                case WAITING:
                    break;
            }
            break;
        default:
            break;
    }
    sm->previous_state=st;
}

void add_state(state_machine * sm, state s){
    put(sm->states,s);
    sm->states_amount++;
}

int remove_state(state_machine * sm, state s){
    if(!remove_node(sm->states,s)){
        sm->states_amount--;
        return 0;
    }
    else{
        return -1;
    }

}