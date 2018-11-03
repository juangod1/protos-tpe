//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_TOPLEVELSTATEMACHINE_H
#define PROTOS_TPE_TOPLEVELSTATEMACHINE_H

#include <sys/select.h>
#include "stateMachine.h"

/* FD List
 *  read_fds[0] = MUA_CONNECT_SOCKET
 */
#define CONNECT_CLIENT_STATE 1

/* FD List
 *  read_fds[0] = MUA_CONNECT_SOCKET
 *  read_fds[1] = connect_origin_sock
 *  read_fds[2] = thread_pipe
 */
#define CONNECT_CLIENT_STAGE_TWO_STATE 2

/* FD List
 *  read_fds[0] = MUA_CONNECT_SOCKET
 *  read_fds[1] = connect_origin_sock
 */
#define CONNECT_CLIENT_STAGE_THREE_STATE 3

/* FD List
 *  read_fds[0] = MUA_CONNECT_SOCKET
 *  read_fds[1] = connect_origin_sock
 */
#define CONNECT_CLIENT_STAGE_FOUR_STATE 7


/* FD List
 *  read_fds[0] = ADMIN_read_fd
 */
#define ATTEND_ADMIN_STATE 4


/* FD List
 *  read_fds[0] = ADMIN_CONNECT_SOCKET
 */
#define CONNECT_ADMIN_STATE 5


/* FD List
 *  read_fds[0] = MUA_read_fd
 *  write_fds[0] = MUA_write_fd
 *  read_fds[1] = ORIGIN_read_fd
 *  write_fds[1] = ORIGIN_write_fd
 *  read_fds[2] = pipe_read_fd
 *  write_fds[2] = pipe_write_fd
 */
#define ATTEND_CLIENT_STATE 6


void disconnect_all(state_machine *sm);

void disconnect(state st);

state_machine *initialize_master_machine(file_descriptor MUA_sock, file_descriptor admin_sock);

void set_up_fd_sets(fd_set *read_fds, fd_set *write_fds);

void disconnect(state st);

execution_state CONNECT_CLIENT_STAGE_THREE_on_arrive(state s, file_descriptor fd, int is_read);

execution_state CONNECT_CLIENT_STAGE_THREE_on_resume(state s, file_descriptor fd, int is_read);

state_code CONNECT_CLIENT_STAGE_THREE_on_leave(state s);

execution_state CONNECT_CLIENT_STAGE_FOUR_on_arrive(state s, file_descriptor fd, int is_read);

execution_state CONNECT_CLIENT_STAGE_FOUR_on_resume(state s, file_descriptor fd, int is_read);

state_code CONNECT_CLIENT_STAGE_FOUR_on_leave(state s);

execution_state CONNECT_CLIENT_STAGE_TWO_on_arrive(state s, file_descriptor fd, int is_read);

execution_state CONNECT_CLIENT_STAGE_TWO_on_resume(state s, file_descriptor fd, int is_read);

state_code CONNECT_CLIENT_STAGE_TWO_on_leave(state s);

execution_state ATTEND_ADMIN_on_arrive(state s, file_descriptor fd, int is_read);

execution_state ATTEND_ADMIN_on_resume(state s, file_descriptor fd, int is_read);

state_code ATTEND_ADMIN_on_leave(state s);

execution_state CONNECT_ADMIN_on_arrive(state s, file_descriptor fd, int is_read);

execution_state CONNECT_ADMIN_on_resume(state s, file_descriptor fd, int is_read);

state_code CONNECT_ADMIN_on_leave(state s);

execution_state CONNECT_CLIENT_on_arrive(state s, file_descriptor fd, int is_read);

execution_state CONNECT_CLIENT_on_resume(state s, file_descriptor fd, int is_read);

state_code CONNECT_CLIENT_on_leave(state s);

execution_state ATTEND_CLIENT_on_arrive(state s, file_descriptor fd, int is_read);

execution_state ATTEND_CLIENT_on_resume(state s, file_descriptor fd, int is_read);

state_code ATTEND_CLIENT_on_leave(state s);

execution_state ERROR_on_arrive(state s, file_descriptor fd, int is_read);

execution_state ERROR_on_resume(state s, file_descriptor fd, int is_read);

state_code ERROR_on_leave(state s);

void debug_print_state(int state);

void log_event(state s, char event, char *data);

#endif //PROTOS_TPE_TOPLEVELSTATEMACHINE_H
