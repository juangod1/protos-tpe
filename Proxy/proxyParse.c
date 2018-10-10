#include "../Shared/include/executionValidator.h"
#include "include/proxyParse.h"
#include "include/optionValidatorFunctions.h"
#include "include/main.h"

int proxy_parse(int argc, char ** argv)
{
    if(argc<2)
    {
        printf("Program execution requires at least one parameter\n");
        return ERROR;
    }
    response_p response = malloc(sizeof(response_t));
    initialize_options();
    if(argc==2)
    {
        int ret=execution_validator(argv[1],response);
        if(!response->success)
        {
            printf("Invalid argument: '%s': %s.\n", argv[1], response->error_text);
            free(response);
            return ERROR;
        }
        free(response);
        return ret;
    }
    parse_command(argc-1,argv,response);
    if(!response->success)
    {
        free(response);
        return ERROR;
    }
    validate_server_string(argv[argc-1],response);
    if(!response->success)
    {
        printf("Invalid server string: '%s': %s.\n", argv[1], response->error_text);
        free(response);
        return ERROR;
    }
    free(response);
    return STANDARD;

}

int execution_validator(char * arg,response_p response)
{
    validate_server_string(arg,response);
    if(response->success==TRUE)
    {
        return STANDARD;
    }
    if (arg[0] != '-' || arg[1]==0 || arg[2]!=0)
    {
        response->success = FALSE;
        response->error_text = "Non valid execution";
        return ERROR;
    }

    char ch=arg[1];

    if(ch=='h')
    {
        response->success = TRUE;
        return HELP;
    }
    if(ch=='v')
    {
        response->success = TRUE;
        return VERSION;
    }

    response->success = FALSE;
    response->error_text = "Non valid execution";
    return ERROR;
}