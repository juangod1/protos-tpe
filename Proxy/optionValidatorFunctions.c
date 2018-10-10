#include "include/optionValidatorFunctions.h"

response_p validate_server_string(char *string, response_p response) //TODO: parse string, make sure it has the correct format.
{
    if(strcmp(string,"-h")==0 ||strcmp(string,"-v")==0)
    {
        response->success=FALSE;
        response->error_text="Invalid server address";
        return response;
    }
    response->success=TRUE;
    response->next_argument=0;
    return response;
}
void error_specification_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void help_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void management_direction_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void replacement_message_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void censored_mediatype_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void management_port_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void local_port_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void origin_port_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void command_specification_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void version_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}