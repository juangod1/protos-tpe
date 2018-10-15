//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_STATESELECTOR_H
#define PROTOS_TPE_STATESELECTOR_H

#define MAX_FD 65535

#include "stateMachine.h"

state_code select_state();
void remove_read_fd(file_descriptor fd);
void remove_write_fd(file_descriptor fd);
void initialize_selector(file_descriptor mua);
void add_read_fd(file_descriptor fd);
void add_write_fd(file_descriptor fd);

#endif //PROTOS_TPE_STATESELECTOR_H
