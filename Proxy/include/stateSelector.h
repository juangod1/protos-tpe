//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_STATESELECTOR_H
#define PROTOS_TPE_STATESELECTOR_H

#include "stateMachine.h"

state_code select_state();
state get_state_by_fd(file_descriptor fd);
void set_waiting_read(file_descriptor fd);
void set_waiting_write(file_descriptor fd);
void remove_read_fd(file_descriptor fd);
void remove_write_fd(file_descriptor fd);

#endif //PROTOS_TPE_STATESELECTOR_H
