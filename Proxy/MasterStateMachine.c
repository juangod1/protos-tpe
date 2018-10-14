//
// Created by juangod on 13/10/18.
//

#include <stdlib.h>
#include "include/state.h"
#include "include/stateMachine.h"
#include "include/MasterStateMachine.h"
#include "include/list.h"

state_machine * sm;


/*
 * ERROR STATE SHOULD ALWAYS BE THE FIRST STATE
 */
state_machine * initialize_master_machine(){
    sm = new_machine();
    sm->states=new_list();
    state s = new_state(ERROR_STATE, ERROR_on_arrive, ERROR_on_resume,ERROR_on_leave);
    s->wait_write_fd=-1;
    s->wait_read_fd=-1;
    put(sm->states,s);
    return sm;
}

execution_state ATTEND_ADMIN_on_arrive(){

}

execution_state ATTEND_ADMIN_on_resume(){

}

state_code ATTEND_ADMIN_on_leave(){

}

execution_state CONNECT_ADMIN_on_arrive(){

}

execution_state CONNECT_ADMIN_on_resume(){

}

state_code CONNECT_ADMIN_on_leave(){

}

execution_state CONNECT_CLIENT_on_arrive(){

}

execution_state CONNECT_CLIENT_on_resume(){

}

state_code CONNECT_CLIENT_on_leave(){

}

execution_state ATTEND_CLIENT_on_arrive(){

}

execution_state ATTEND_CLIENT_on_resume(){

}

state_code ATTEND_CLIENT_on_leave(){

}

execution_state ERROR_on_arrive(){

}

execution_state ERROR_on_resume(){

}

state_code ERROR_on_leave(){

}