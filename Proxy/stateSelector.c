//
// Created by juangod on 13/10/18.
//
#include <sys/param.h>
#include "include/stateMachine.h"
#include "include/stateSelector.h"
#include "include/MasterStateMachine.h"
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

/*
 * Return value: Char array { file_descriptor, is_read }
 */
void select_state(int ret[2]){
    set_up_fd_sets(&read_fds,&write_fds);

    int select_ret;
    select_ret = pselect(MAX_FD+1,&read_fds,&write_fds,NULL,&timeout,NULL);
    if(select_ret == -1)
    {
        perror("pselect error.");
        error();
    }

    int i;
    for(i=0;i<MAX_FD;i++){
        if(FD_ISSET(i,&read_fds)){
            ret[0]=i;
            ret[1]=1;
            return;
        }
        if(FD_ISSET(i,&write_fds)){
            ret[0]=i;
            ret[1]=0;
            return;
        }
    }
}

void remove_read_fd(file_descriptor fd){
    FD_CLR(fd, &read_fds);
}

void remove_write_fd(file_descriptor fd){
    FD_CLR(fd, &write_fds);
}

void initialize_selector(file_descriptor mua){
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    MUA_sock = mua;

    add_read_fd(mua);
}