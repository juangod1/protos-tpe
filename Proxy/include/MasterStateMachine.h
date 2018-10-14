//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_TOPLEVELSTATEMACHINE_H
#define PROTOS_TPE_TOPLEVELSTATEMACHINE_H

#define SELECT_STATE 1
#define CONNECT_CLIENT_STATE 2
#define ATTEND_ADMIN_STATE 3
#define CONNECT_ADMIN_STATE 4
#define ATTEND_CLIENT_STATE 5
#define ERROR_STATE 6

state_machine * initialize_master_machine(file_descriptor MUA_sock);
execution_state ATTEND_ADMIN_on_arrive();
execution_state ATTEND_ADMIN_on_resume();
state_code ATTEND_ADMIN_on_leave();
execution_state CONNECT_ADMIN_on_arrive();
execution_state CONNECT_ADMIN_on_resume();
state_code CONNECT_ADMIN_on_leave();
execution_state CONNECT_CLIENT_on_arrive();
execution_state CONNECT_CLIENT_on_resume();
state_code CONNECT_CLIENT_on_leave();
execution_state ATTEND_CLIENT_on_arrive();
execution_state ATTEND_CLIENT_on_resume();
state_code ATTEND_CLIENT_on_leave();
execution_state ERROR_on_arrive();
execution_state ERROR_on_resume();
state_code ERROR_on_leave();

#endif //PROTOS_TPE_TOPLEVELSTATEMACHINE_H
