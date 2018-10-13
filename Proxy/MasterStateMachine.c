//
// Created by juangod on 13/10/18.
//

#include <malloc.h>
#include "include/stateMachine.h"
#include "include/MasterStateMachine.h"

state_machine * sm;

state_machine * initialize_master_machine(){
    int states_amount=6;
    state ** states = malloc(sizeof(*states)*states_amount);
    int i;
    states[0]= new_state(SELECT_STATE, SELECT_on_arrive, SELECT_on_resume,SELECT_on_leave);
    states[1]= new_state(ATTEND_ADMIN_STATE, ATTEND_ADMIN_on_arrive, ATTEND_ADMIN_on_resume,ATTEND_ADMIN_on_leave);
    states[2]= new_state(CONNECT_ADMIN_STATE, CONNECT_ADMIN_on_arrive, CONNECT_ADMIN_on_resume,CONNECT_ADMIN_on_leave);
    states[3]= new_state(CONNECT_CLIENT_STATE, CONNECT_CLIENT_on_arrive, CONNECT_CLIENT_on_resume,CONNECT_CLIENT_on_leave);
    states[4]= new_state(ATTEND_CLIENT_STATE, ATTEND_CLIENT_on_arrive, ATTEND_CLIENT_on_resume,ATTEND_CLIENT_on_leave);
    states[5]= new_state(ERROR_STATE, ERROR_on_arrive, ERROR_on_resume,ERROR_on_leave);
    sm = new_machine(states_amount,states,SELECT_STATE);
    return sm;
}

execution_state SELECT_on_arrive(){

}

execution_state SELECT_on_resume(){

}

state_code SELECT_on_leave(){

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