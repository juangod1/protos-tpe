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
#include "../Shared/include/buffer.h"
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
    buffer_initialize(&(st->buffers[0]), BUFFER_SIZE);
    buffer_initialize(&(st->buffers[1]), BUFFER_SIZE);
    buffer_initialize(&(st->buffers[2]), BUFFER_SIZE);

    add_state(sm,st);

    return NOT_WAITING;
}

execution_state CONNECT_CLIENT_on_resume(state s, file_descriptor fd, int is_read){

}

state_code CONNECT_CLIENT_on_leave(state s){

}

execution_state ATTEND_CLIENT_on_arrive(state s, file_descriptor fd, int is_read){
    switch(is_read) {
        case 1: // True
            if (s->read_fds[0] == fd)
            {
                buffer_read(fd,s->buffers[0]);
            }
            if (s->read_fds[1] == fd)
            {
                buffer_read(fd,s->buffers[1]);
            }
            if (s->read_fds[2] == fd)
            {
                buffer_read(fd,s->buffers[2]);
            }
            break;
        case 0: // False
            if (s->write_fds[0] == fd)
            {
                buffer_write(fd,s->buffers[0]);
                // Mua Write
            }
            if (s->write_fds[1] == fd)
            {
                buffer_write(fd,s->buffers[1]);
            }
            if (s->write_fds[2] == fd)
            {
                buffer_write(fd,s->buffers[2]);
            }
            break;
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

    if(curr->st->buffers[0]!=NULL)
    {
        if(buffer_is_empty(curr->st->buffers[0])){
            if(curr->st->read_fds[0]>0)
                add_read_fd(curr->st->read_fds[0]); // MUA read
        }
        else{
            if(curr->st->write_fds[1]>0)
                add_write_fd(curr->st->write_fds[1]); // Origin write
        }
    }
    if(curr->st->buffers[1]!=NULL)
    {
        if(buffer_is_empty(curr->st->buffers[1])){
            if(curr->st->read_fds[1]>0)
                add_read_fd(curr->st->read_fds[1]); // ORIGIN read
        }
        else{
            if(curr->st->write_fds[2]>0)
                add_write_fd(curr->st->write_fds[2]); // Transform write
        }
    }
    if(curr->st->buffers[2]!=NULL)
    {
        if(buffer_is_empty(curr->st->buffers[2])){
            if(curr->st->read_fds[2]>0)
                add_read_fd(curr->st->read_fds[2]); // Transform read
        }
        else{
            if(curr->st->write_fds[0]>0)
                add_write_fd(curr->st->write_fds[0]); // MUA write
        }
    }

    set_up_fd_sets_rec(read_fds,write_fds,curr->next);
}

void set_up_fd_sets(fd_set * read_fds, fd_set * write_fds){
    if(sm==NULL)
    {
        printf("Found null\n");fflush(stdout);
    }
    if(sm->states==NULL)
    {
        printf("Found null2\n");fflush(stdout);
    }
    set_up_fd_sets_rec(read_fds,write_fds,sm->states->head);
}