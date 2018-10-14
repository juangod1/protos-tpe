//
// Created by juangod on 13/10/18.
//
#include <sys/param.h>
#include "include/stateMachine.h"
#include <sys/select.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/socket.h>

fd_set read_fds;
fd_set write_fds;

const struct timespec timeout={ //TODO: como hacer que no timeoutee?
        .tv_sec=99999999999, .tv_nsec=0
};

file_descriptor MUA_sock;

void add_read_fd(file_descriptor fd){
    FD_SET(fd, &read_fds);
}

void add_write_fd(file_descriptor fd){
    FD_SET(fd, &write_fds);
}

state_code select_state(){

    int select_ret;
    select_ret = pselect(MUA_sock+1,&read_fds,&write_fds,NULL,&timeout,NULL);
    if(select_ret == -1)
    {
        perror("pselect error.");
        error();
    }
    
    return select_ret;
}

void set_waiting_read(file_descriptor fd, state st){
    int i;
    for(i=0;i<FD_SETSIZE;i++){
        if(FD_ISSET(st->wait_read_fd,&read_fds)){
            FD_CLR(st->wait_read_fd,&read_fds);
            break;
        }
        if(FD_ISSET(st->wait_write_fd,&write_fds)){
            FD_CLR(st->wait_write_fd,&write_fds);
            break;
        }
    }
    FD_SET(fd,&read_fds);
}

void set_waiting_write(file_descriptor fd, state st){
    int i;
    for(i=0;i<FD_SETSIZE;i++){
        if(FD_ISSET(st->wait_read_fd,&read_fds)){
            FD_CLR(st->wait_read_fd,&read_fds);
            break;
        }
        if(FD_ISSET(st->wait_write_fd,&write_fds)){
            FD_CLR(st->wait_write_fd,&write_fds);
            break;
        }
    }
    FD_SET(fd,&write_fds);
}

void remove_read_fd(file_descriptor fd){
    FD_CLR(fd, &read_fds);
}

void remove_write_fd(file_descriptor fd){
    FD_CLR(fd, &write_fds);
}

void initialize_selector(file_descriptor mua){
    const struct timespec timeout={
            .tv_sec=5, .tv_nsec=0
    };

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    MUA_sock = mua;

    add_read_fd(mua);
}