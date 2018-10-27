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
#include <signal.h>

fd_set read_fds;
fd_set write_fds;

void add_read_fd(file_descriptor fd){
    printf("Adding fd %d to selector as READ\n",fd);fflush(stdout);
    FD_SET(fd, &read_fds);
}

void add_write_fd(file_descriptor fd){
    printf("Adding fd %d to selector as WRITE\n",fd);fflush(stdout);
    FD_SET(fd, &write_fds);
}

void signal_handler(int signal){

}

/*
 * Return value: Char array { file_descriptor, is_read }
 */
void select_state(int * ret){
    // https://lwn.net/Articles/176911/
    struct sigaction sa;
    sigset_t emptyset, blockset;

    sigemptyset(&blockset);
    sigaddset(&blockset, SIGINT);
    sigprocmask(SIG_BLOCK, &blockset, NULL);

    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    set_up_fd_sets(&read_fds,&write_fds);
    int select_ret;

    sigemptyset(&emptyset);

    select_ret = pselect(MAX_FD+1,&read_fds,&write_fds,NULL,NULL,&emptyset);
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

void initialize_selector(){
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
}