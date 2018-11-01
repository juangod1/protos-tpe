#ifndef PROXYCOMMUNICATION_H
#define PROXYCOMMUNICATION_H

#include <unistd.h>
#include "state.h"

#define NUM_PIPES          2

#define PROXY_READ_PIPE   0
#define PROXY_WRITE_PIPE  1

#define READ_FD  0
#define WRITE_FD 1

#define PROXY_READ_FD  ( pipes[PROXY_READ_PIPE][READ_FD]   )
#define PARSER_WRITE_FD ( pipes[PROXY_READ_PIPE][WRITE_FD] )

#define PARSER_READ_FD  ( pipes[PROXY_WRITE_PIPE][READ_FD]   )
#define PROXY_WRITE_FD ( pipes[PROXY_WRITE_PIPE][WRITE_FD] )


int start_parser(char *cmd, int pipe_ret[2], state s);

int check_parser_exit_status(int parser_pid);

#endif