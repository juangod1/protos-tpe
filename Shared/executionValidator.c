#include "include/executionValidator.h"

option_t options[MAX_OPTIONS];
int options_size = 0;

response_p parse_command(int argc, char ** argv, response_p response)
{
    return validate_arguments(argc, argv, response);
}

response_p validate_arguments(int argc, char ** argv,response_p response)
{
    argc--;
    argv+=1;
    int counter=0;

    while(counter<argc)
    {
        validate_argument(argc-counter, argv+counter,response);
        if(!response->success)
        {
            printf("Invalid argument: '%s': %s.\n", argv[counter], response->error_text);
            return response;
        }
        int argument_shift = 1 +response->next_argument;;
        counter+=argument_shift;
    }
    return response;
}

response_p validate_argument(int argc, char ** argv, response_p response)
{
    if (argv[0][0] != '-' || argv[0][1]==0 || argv[0][2]!=0)
    {
        response->success = FALSE;
        response->error_text = "Argument should be an option";
        return response;
    }
    option_p option = NULL;
    option_validator(argv[0][1], &option);
    if (option == NULL)
    {
        response->success = FALSE;
        response->error_text = "Argument is not a valid option";
        return response;
    }
    int remaining_args= argc-1;
    if (remaining_args < option->validator_arguments) {
        response->success = FALSE;
        response->error_text = "Not enough arguments for option";
        return response;
    }
    argc--;
    argv++;
    option->validator(argc, argv, response);
    if(response->success)
    {
        if(option->validator_arguments==1)
        {
            option->function_argument=*argv;
            response->next_argument=1;
        }
        else
        {
            option->function_argument=NULL;
            response->next_argument=0;
        }
    }
    return response;
}

void option_validator(char ch, option_p * option)
{
    int i;
    for(i=0;i<options_size;i++)
    {
        if(ch == options[i].command)
        {
            if(options[i].selected==1)
            {
                *option=NULL;
                return;
            }
            options[i].selected=1;
            *option=&options[i];
            return;
        }
    }
    *option=NULL;
}


void execute_options()
{
    for( int i=0; i<options_size; i++)
    {
        option_t option = options[i];
        void (*execute_option)(char * arg) = option.function;
        execute_option(option.function_argument);
    }
}

void option_register(char * name, char command, void (*function)(char * arg), char * function_argument ,char * description, void (*validator)(int argc, char ** argv, response_p resp), int validator_arguments)
{
    option_t option = {0};
    option.name = name;
    option.command = command;
    option.function = function;
    option.function_argument = function_argument;
    option.description = description;
    option.selected=FALSE;
    option.validator=validator;
    option.validator_arguments=validator_arguments;
    options[options_size++]=option;
}

void option_help()
{
    for(int i=0; i<options_size; i++)
    {
        option_t option=options[i];
        printf("\t-%c:\t%-30s - %s\n",option.command,option.name, option.description);
    }
}