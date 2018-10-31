#ifndef ARGUMENTVALIDATOR_H
#define ARGUMENTVALIDATOR_H

#define MAX_OPTIONS 12
#define FALSE 0
#define TRUE 1


typedef struct
{
	char *error_text;
	char success;
	int  next_argument;
} response_t;

typedef response_t *response_p;

typedef struct
{
	char *name;
	char command;

	void (*function)(char *arg);

	char *function_argument;
	char *description;
	char selected;

	void (*validator)(int argc, char **argv, response_p resp);

	int validator_arguments;
}                  option_t;

typedef option_t *option_p;

response_p parse_command(int argc, char **argv, response_p response);

response_p validate_arguments(int argc, char **argv, response_p response);

response_p validate_argument(int argc, char **argv, response_p response);

void option_register(char *name, char command, void (*function)(char *arg), char *function_argument, char *description,
                     void (*validator)(int argc, char **argv, response_p resp), int validator_arguments);

void option_validator(char ch, option_p *option);

void option_help();

void execute_options();


#include <stdlib.h>
#include <stdio.h>
#include "../../Proxy/include/main.h"
#include "../../Proxy/include/optionValidatorFunctions.h"
#include "../../Proxy/include/options.h"


#endif
