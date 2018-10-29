//
// Created by juangod on 29/10/18.
//

#ifndef PROTOS_TPE_ERROR_H
#define PROTOS_TPE_ERROR_H

#include "state.h"
#include "stateMachine.h"

void error_print(char * s);
void error_terminal();
void init_error(state_machine * s);
void error_disconnect_client(state s);

#endif //PROTOS_TPE_ERROR_H
