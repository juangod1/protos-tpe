#include "include/executionValidator.h"

static option_t options[MAX_OPTIONS];
static int options_size = 0;

void initialize_options()
{
    option_register("Test Mode","t",test_mode,"A mode that tests.");
    option_register("Non test Mode","n",non_test_mode,"A mode that doesn't test");
}

void option_register(char * name, char * command, void (*function)(), char * description)
{
    option_t option = {0};
    option.name = name;
    option.command = command;
    option.function = function;
    option.description = description;
    options[options_size++]=option;
}

void execute_options(int *selectedOptions, int optionLength)
{
    for( int i=0; i<optionLength; i++)
    {
        void (*execute_option)() = options[selectedOptions[i]].function;
        execute_option();
    }
}


validation_response_t validate_arguments(int argc, char ** argv)
{
    int optionLength=0;
    int count;
    validation_response_t ret = {0};
    for(count=1;count<argc;count++)
    {
        int response = validate_argument(argv[count]);
        if(response<0)
        {
            printf("Invalid argument: '%s'\n", argv[count]);
            ret.length=-1;
            return ret;
        }
        ret.selectedOptions[optionLength++]=response;
    }
    ret.length=optionLength;
    return ret;
}

int option_validator(char option)
{
    int i;
    for(i=0;i<options_size;i++)
    {
        if(option == options[i].command[0]) return i;
    }
    return -1;
}

int validate_argument(char * arg)
{
    if(arg==NULL) return -1;

    if(arg[0]=='-')
    {
        return option_validator(arg[1]);
    }

    return -1;
}
