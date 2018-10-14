//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_STATESELECTOR_H
#define PROTOS_TPE_STATESELECTOR_H

#include "stateMachine.h"

state get_state_by_fd(file_descriptor fd, state_machine * stm);
state_code select_state();
void set_waiting_read(file_descriptor fd, state st);
void set_waiting_write(file_descriptor fd, state st);
void remove_read_fd(file_descriptor fd);
void remove_write_fd(file_descriptor fd);
void initialize_selector();

#endif //PROTOS_TPE_STATESELECTOR_H
