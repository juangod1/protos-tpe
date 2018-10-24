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
#include "include/options.h"
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

state_machine * sm;

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

    add_state(sm,s);

    state connect_client = new_state(CONNECT_CLIENT_STATE, CONNECT_CLIENT_on_arrive, CONNECT_CLIENT_on_resume,CONNECT_CLIENT_on_leave);
    connect_client->read_fds[0]=MUA_sock;
    add_state(sm,connect_client);

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
        int accept_ret = accept(s->read_fds[0],NULL,NULL);

        if(accept_ret<0){
            perror("accept");
            error();
        }

        state st = new_state(CONNECT_CLIENT_STAGE_TWO_STATE,CONNECT_CLIENT_STAGE_TWO_on_arrive,CONNECT_CLIENT_STAGE_TWO_on_resume,CONNECT_CLIENT_STAGE_TWO_on_leave);
        st->read_fds[0]=accept_ret;
        st->read_fds[1]=setup_origin_socket();

        add_state(sm,st);

        return NOT_WAITING;
}

execution_state CONNECT_CLIENT_on_resume(state s, file_descriptor fd, int is_read){

}

state_code CONNECT_CLIENT_on_leave(state s){

}

execution_state CONNECT_CLIENT_STAGE_TWO_on_arrive(state s, file_descriptor fd, int is_read){
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_port = htons((uint16_t)get_app_context()->origin_port);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(get_app_context()->server_string);

    //Connect to remote server
    if (connect(s->read_fds[1], (struct sockaddr *)&address , sizeof(address)) < 0)
    {
        perror("Connect to origin error");
        error();
    }

    state st = new_state(ATTEND_CLIENT_STATE,ATTEND_CLIENT_on_arrive,ATTEND_CLIENT_on_resume,ATTEND_CLIENT_on_leave);
    st->read_fds[0] = s->read_fds[0];
    st->write_fds[0] = s->read_fds[0];
    st->read_fds[1] = fd;
    st->write_fds[1] = fd;
    buffer_initialize(&(st->buffers[0]), BUFFER_SIZE);
    buffer_initialize(&(st->buffers[1]), BUFFER_SIZE);
    buffer_initialize(&(st->buffers[2]), BUFFER_SIZE);

    add_state(sm,st);

    return NOT_WAITING;
}

execution_state CONNECT_CLIENT_STAGE_TWO_on_resume(state s, file_descriptor fd, int is_read){

}

state_code CONNECT_CLIENT_STAGE_TWO_on_leave(state s){
    remove_state(sm,s);
}

execution_state ATTEND_CLIENT_on_arrive(state s, file_descriptor fd, int is_read){
    switch(is_read) {
        case 1: // True
            if (s->read_fds[0] == fd)
            {   // MUA READ
                buffer_read(fd,s->buffers[0]);
                printf("--------------------------------------------------------\n");
                printf("Read buffer content from MUA: \n");
                print_buffer(s->buffers[0]);
            }
            else if (s->read_fds[1] == fd)
            {   // Origin READ
                buffer_read(fd,s->buffers[1]);
                printf("--------------------------------------------------------\n");
                printf("Read buffer content from Origin: \n");
                print_buffer(s->buffers[1]);
            }
            else if (s->read_fds[2] == fd)
            {   // Transform READ
                buffer_read(fd,s->buffers[2]);
                printf("--------------------------------------------------------\n");
                printf("Read buffer content from Transform: \n");
                print_buffer(s->buffers[2]);
            }
            break;
        case 0: // False
            if (s->write_fds[0] == fd)
            {   // MUA WRITE
                printf("--------------------------------------------------------\n");
                printf("Wrote buffer content to MUA: \n");
                print_buffer(s->buffers[2]);
                buffer_write(fd,s->buffers[2]);
            }
            else if (s->write_fds[1] == fd)
            {   // Origin WRITE
                printf("--------------------------------------------------------\n");
                printf("Wrote buffer content to Origin: \n");
                print_buffer(s->buffers[0]);
                buffer_write(fd,s->buffers[0]);
            }
            else if (s->write_fds[2] == fd)
            {   // Transform WRITE
                printf("--------------------------------------------------------\n");
                printf("Wrote buffer content to Transform: \n");
                print_buffer(s->buffers[1]);
                buffer_write(fd,s->buffers[1]);
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

    switch(curr->st->id){
        case ATTEND_CLIENT_STATE:
            if(curr->st->buffers[0]!=NULL)
            {
                if(buffer_is_empty(curr->st->buffers[0])){
                    if(curr->st->read_fds[0]>0) {
                        printf("Buffer 1 is empty ==> ");
                        add_read_fd(curr->st->read_fds[0]); // MUA read
                    }
                }
                else{
                    if(curr->st->write_fds[1]>0) {
                        printf("Buffer 1 is not empty ==> ");
                        add_write_fd(curr->st->write_fds[1]); // Origin write
                    }
                }
            }
            if(curr->st->buffers[1]!=NULL)
            {
                if(buffer_is_empty(curr->st->buffers[1])){
                    if(curr->st->read_fds[1]>0) {
                        printf("Buffer 2 is empty ==> ");
                        add_read_fd(curr->st->read_fds[1]); // ORIGIN read
                    }
                }
                else{
                    if(curr->st->write_fds[2]>0) {
                        printf("Buffer 2 is not empty ==> ");
                        add_write_fd(curr->st->write_fds[2]); // Transform write
                    }
                }
            }
            if(curr->st->buffers[2]!=NULL)
            {
                if(buffer_is_empty(curr->st->buffers[2])){
                    if(curr->st->read_fds[2]>0) {
                        printf("Buffer 3 is empty ==> ");
                        add_read_fd(curr->st->read_fds[2]); // Transform read
                    }
                }
                else{
                    if(curr->st->write_fds[0]>0) {
                        printf("Buffer 3 is not empty ==> ");
                        add_write_fd(curr->st->write_fds[0]); // MUA write
                    }
                }
            }
            break;
        case CONNECT_CLIENT_STATE:
            add_read_fd(curr->st->read_fds[0]);
            break;
        case CONNECT_CLIENT_STAGE_TWO_STATE:
            add_read_fd(curr->st->read_fds[1]);
            break;
        default:
            ;int i;
            for(i=0;i<3;i++){
                if(curr->st->read_fds[i]!=-1&&curr->st->read_fds[i]!=-2)
                    add_read_fd(curr->st->read_fds[i]);
            }
            for(i=0;i<3;i++){
                if(curr->st->write_fds[i]!=-1&&curr->st->write_fds[i]!=-2)
                    add_write_fd(curr->st->write_fds[i]);
            }
            break;
    }

    set_up_fd_sets_rec(read_fds,write_fds,curr->next);
}

void set_up_fd_sets(fd_set * read_fds, fd_set * write_fds){
    FD_ZERO(read_fds);
    FD_ZERO(write_fds);
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

void debug_print_state(int state){
    char * msg;
    switch(state){
        case CONNECT_CLIENT_STATE:
            msg="CONNECT_CLIENT_STATE";
            break;
        case CONNECT_CLIENT_STAGE_TWO_STATE:
            msg="CONNECT_CLIENT_STAGE_TWO_STATE";
            break;
        case ATTEND_ADMIN_STATE:
            msg="ATTEND_ADMIN_STATE";
            break;
        case CONNECT_ADMIN_STATE:
            msg="CONNECT_ADMIN_STATE";
            break;
        case ATTEND_CLIENT_STATE:
            msg="ATTEND_CLIENT_STATE";
            break;
        case ERROR_STATE:
            msg="ERROR_STATE";
            break;
        default:
            msg="State not found in debug print";
            break;
    }
    printf("State %s was chosen.\n",msg);fflush(stdout);
}