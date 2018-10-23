//
// Created by juangod on 13/10/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "include/stateMachine.h"
#include "include/stateSelector.h"
#include "include/state.h"
#include "include/MasterStateMachine.h"

state new_state(state_code id, execution_state (*on_arrive)(state s, file_descriptor fd, int is_read), execution_state (*on_resume)(state s, file_descriptor fd, int is_read), state_code (*on_leave)(state s)){
    state new = malloc(sizeof(struct stateStruct));
    int i;
    for(i=0;i<3;i++){
        new->read_fds[i]=-1;
    }
    for(i=0;i<3;i++){
        new->write_fds[i]=-1;
    }
    new->id = id;
    new->error = 0;
    new->processing_mail=0;
    new->on_arrive = on_arrive;
    new->on_resume = on_resume;
    new->on_leave = on_leave;
    new->exec_state = NOT_WAITING;
    new->buffers[0]=NULL;
    new->buffers[1]=NULL;
    new->buffers[2]=NULL;
    new->persistent_data=0;
    return new;
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

    int next[2]={0};
    select_state(next);

    state st = get(sm->states,next[0],next[1]);
    if(st==NULL){
        perror("Error, no state with file descriptor found.");
    }
    debug_print_state(st->id);

    switch(st->exec_state)
    {
        case NOT_WAITING:
            switch(st->on_arrive(st, next[0], next[1])){
                case NOT_WAITING:
                    st->on_leave(st);
                    break;
                case WAITING:
                    break;
            }
            break;
        case WAITING:
            switch(st->on_resume(st, next[0], next[1])){
                case NOT_WAITING:
                    st->on_leave(st);
                    break;
                case WAITING:
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
    if(!remove_node(sm->states,s)) {
        sm->states_amount--;
        return 0;
    }
    else{
        return -1;
    }

}