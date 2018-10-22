//
// Created by juangod on 13/10/18.
//

#ifndef PROTOS_TPE_STATESELECTOR_H
#define PROTOS_TPE_STATESELECTOR_H

#define MAX_FD 65535

#include "stateMachine.h"

void select_state(int ret[2]);
void remove_read_fd(file_descriptor fd);
void remove_write_fd(file_descriptor fd);
void initialize_selector();
void add_read_fd(file_descriptor fd);
void add_write_fd(file_descriptor fd);

#endif //PROTOS_TPE_STATESELECTOR_H
