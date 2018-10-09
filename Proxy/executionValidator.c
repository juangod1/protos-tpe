#include "include/executionValidator.h"

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