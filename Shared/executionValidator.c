#include "include/executionValidator.h"

option_t options[MAX_OPTIONS];
int options_size = 0;

int validate_arguments(int argc, char ** argv)
{
    int count;

    for(count=1;count<argc;count++)
    {
        int response = validate_argument(argv[count]);
        if(response<0)
        {
            printf("Invalid argument: '%s'\n", argv[count]);
            return -1;
        }
    }
    return 0;
}

int multiple_option_validator(char * str)
{
    if(str==NULL)
    {
        return -1;
    }
    char c= *str;
    int index=0;
    while(c!=0)
    {
        int response = option_validator(c);
        if(response<0)
        {
            return -1;
        }
        index++;
        c=*(str+index);
    }
    return 0;
}

int validate_argument(char * arg)
{
    if(arg==NULL)
    {
        return -1;
    }

    if(arg[0]=='-')
    {
        return multiple_option_validator(arg+1);
    }
    return -1;
}

void execute_options()
{
    for( int i=0; i<options_size; i++)
    {
        option_t option = options[i];
        if(option.selected)
        {
            void (*execute_option)() = option.function;
            execute_option();
        }
    }
}

void option_register(char * name, char command, void (*function)(), char * description)
{
    option_t option = {0};
    option.name = name;
    option.command = command;
    option.function = function;
    option.description = description;
    option.selected=0;
    options[options_size++]=option;
}

int option_validator(char option)
{
    int i;
    for(i=0;i<options_size;i++)
    {
        if(option == options[i].command)
        {
            if(options[i].selected==1)
            {
                return -1;
            }
            options[i].selected=1;
            return i;
        }
    }
    return -1;
}

void help()
{
    for(int i=0; i<options_size; i++)
    {
        option_t option=options[i];
        printf("'-%c':\t%-30s - %s\n",option.command,option.name, option.description);
    }
}