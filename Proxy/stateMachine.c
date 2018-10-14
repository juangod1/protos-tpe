//
// Created by juangod on 13/10/18.
//
#include <stdlib.h>
#include <stdio.h>
#include "include/stateMachine.h"
#include "include/stateSelector.h"
#include "include/state.h"

state new_state(state_code id, execution_state (*on_arrive)(), execution_state (*on_resume)(), state_code (*on_leave)()){
    state new = malloc(sizeof(struct stateStruct));
    new->id = id;
    new->error = 0;
    new->on_arrive = on_arrive;
    new->on_resume = on_resume;
    new->on_leave = on_leave;
    new->exec_state = NOT_WAITING;
    new->internal_state=0;
    return new;
}

void free_state(state st){
    free(st);
}

void free_machine(state_machine * machine){
    int i;
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
        err->on_arrive();
        err->on_resume();
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
            switch(st->on_arrive()){
                case NOT_WAITING:
                    st->on_leave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd, st);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd, st);
                    break;
            }
            break;
        case WAITING_READ:
            switch(st->on_resume()){
                case NOT_WAITING:
                    st->on_leave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd, st);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd, st);
                    break;
            }
            break;
        case WAITING_WRITE:
            switch(st->on_resume()){
                case NOT_WAITING:
                    st->on_leave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd, st);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd, st);
                    break;
            }
            break;
        default:
            break;
    }
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