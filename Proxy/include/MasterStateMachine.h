//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_TOPLEVELSTATEMACHINE_H
#define PROTOS_TPE_TOPLEVELSTATEMACHINE_H

#include <sys/select.h>

#define SELECT_STATE 1
#define CONNECT_CLIENT_STATE 2
#define ATTEND_ADMIN_STATE 3
#define CONNECT_ADMIN_STATE 4
#define ATTEND_CLIENT_STATE 5
#define ERROR_STATE 6

#define ATTEND_CLIENT_READ_MUA 1
#define WRITE_ORIGIN 2
#define READ_ORIGIN 3
#define OK_MUA 4
#define SEND_TRANSFORM 5
#define RECV_TRANSFORM 6
#define WRITE_MUA 7

state_machine * initialize_master_machine(file_descriptor MUA_sock);
void set_up_fd_sets(fd_set * read_fds, fd_set * write_fds);

execution_state ATTEND_ADMIN_on_arrive(state s);
execution_state ATTEND_ADMIN_on_resume(state s);
state_code ATTEND_ADMIN_on_leave(state s);
execution_state CONNECT_ADMIN_on_arrive(state s);
execution_state CONNECT_ADMIN_on_resume(state s);
state_code CONNECT_ADMIN_on_leave(state s);
execution_state CONNECT_CLIENT_on_arrive(state s);
execution_state CONNECT_CLIENT_on_resume(state s);
state_code CONNECT_CLIENT_on_leave(state s);
execution_state ATTEND_CLIENT_on_arrive(state s);
execution_state ATTEND_CLIENT_on_resume(state s);
state_code ATTEND_CLIENT_on_leave(state s);
execution_state ERROR_on_arrive(state s);
execution_state ERROR_on_resume(state s);
state_code ERROR_on_leave(state s);

#endif //PROTOS_TPE_TOPLEVELSTATEMACHINE_H
