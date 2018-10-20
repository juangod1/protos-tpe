//
// Created by juangod on 13/10/18.
//

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "include/state.h"
#include "include/stateMachine.h"
#include "include/MasterStateMachine.h"
#include "include/list.h"
#include "include/stateSelector.h"
#include <sys/select.h>

state_machine * sm;

file_descriptor MUA_sock;

state_machine * initialize_master_machine(file_descriptor MUA_sock){
    sm = new_machine();
    sm->states=new_list();

    sm->states_amount=0;
    sm->previous_state=NULL;
    sm->next_state=NULL;

    state s = new_state(ERROR_STATE, ERROR_on_arrive, ERROR_on_resume,ERROR_on_leave);
    // Error state has fds set as -2
    int i;
    for(i=0;i<3;i++){
        s->read_fds[i]=-2;
    }
    for(i=0;i<2;i++){
        s->write_fds[i]=-2;
    }

    put(sm->states,s);

    state connect_client = new_state(CONNECT_CLIENT_STATE, CONNECT_CLIENT_on_arrive, CONNECT_CLIENT_on_resume,CONNECT_CLIENT_on_leave);
    connect_client->read_fds[0]=MUA_sock;
    put(sm->states,connect_client);

    return sm;
}

execution_state ATTEND_ADMIN_on_arrive(state s, file_descriptor fd, int is_read){

}

execution_state ATTEND_ADMIN_on_resume(state s, file_descriptor fd, int is_read){

}

state_code ATTEND_ADMIN_on_leave(state s){

}

execution_state CONNECT_ADMIN_on_arrive(state s, file_descriptor fd, int is_read){

}

execution_state CONNECT_ADMIN_on_resume(state s, file_descriptor fd, int is_read){

}

state_code CONNECT_ADMIN_on_leave(state s){

}

execution_state CONNECT_CLIENT_on_arrive(state s, file_descriptor fd, int is_read){
    int accept_ret = accept(MUA_sock,NULL,NULL);

    if(accept_ret<0){
        perror("accept");
        error();
    }

    state st = new_state(ATTEND_CLIENT_STATE,ATTEND_CLIENT_on_arrive,ATTEND_CLIENT_on_resume,ATTEND_CLIENT_on_leave);
    st->read_fds[0] = accept_ret;
    add_state(sm,st);



    return NOT_WAITING;
}

execution_state CONNECT_CLIENT_on_resume(state s, file_descriptor fd, int is_read){

}

state_code CONNECT_CLIENT_on_leave(state s){

}

execution_state ATTEND_CLIENT_on_arrive(state s, file_descriptor fd, int is_read){
    if(fd==s->read_fds[0]) {

    }
    if(fd==s->read_fds[1]) {

    }
    if(fd==s->read_fds[2]) {

    }
    if(fd==s->write_fds[0]) {

    }
    if(fd==s->write_fds[1]) {

    }
}

execution_state ATTEND_CLIENT_on_resume(state s, file_descriptor fd, int is_read){
    ATTEND_CLIENT_on_arrive(s,fd, is_read);
}

state_code ATTEND_CLIENT_on_leave(state s){

}

execution_state ERROR_on_arrive(state s, file_descriptor fd, int is_read){

}

execution_state ERROR_on_resume(state s, file_descriptor fd, int is_read){

}

state_code ERROR_on_leave(state s){

}

void set_up_fd_sets_rec(fd_set * read_fds, fd_set * write_fds, node curr){
    if(curr==NULL)
        return;

    int i;
    for(i=0;i<3;i++){
        if(curr->st->read_fds[i]!=-1&&curr->st->read_fds[i]!=-2){
            add_read_fd(curr->st->read_fds[i]);
        }
    }
    for(i=0;i<2;i++){
        if(curr->st->write_fds[i]!=-1&&curr->st->write_fds[i]!=-2){
            add_write_fd(curr->st->write_fds[i]);
        }
    }

    set_up_fd_sets_rec(read_fds,write_fds,curr->next);
}

void set_up_fd_sets(fd_set * read_fds, fd_set * write_fds){
    set_up_fd_sets_rec(read_fds,write_fds,sm->states->head);
}