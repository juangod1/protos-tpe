#ifndef OPTIONVALIDATORFUNCTIONS_H
#define OPTIONVALIDATORFUNCTIONS_H

#include "../../Shared/include/executionValidator.h"
#include <string.h>

response_p validate_server_string(char *string, response_p response);


void error_specification_validation(int argc, char ** argv, response_p resp);
void pop3_direction_validation(int argc, char ** argv, response_p resp);
void management_direction_validation(int argc, char ** argv, response_p resp);
void replacement_message_validation(int argc, char ** argv, response_p resp);
void censored_mediatype_validation(int argc, char ** argv, response_p resp);
void management_port_validation(int argc, char ** argv, response_p resp);
void local_port_validation(int argc, char ** argv, response_p resp);
void origin_port_validation(int argc, char ** argv, response_p resp);
void command_specification_validation(int argc, char ** argv, response_p resp);
void test_mode_validation(int argc, char ** argv, response_p resp);
void non_test_mode_validation(int argc, char ** argv, response_p resp);


#endif
