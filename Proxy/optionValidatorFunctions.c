#include "include/optionValidatorFunctions.h"
#include "../Shared/include/lib.h"

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
void pop3_direction_validation(int argc, char ** argv, response_p resp)//TODO: parse string, make sure it has the correct format.
{
    resp->success=TRUE;
    resp->next_argument=1;
}
void error_specification_validation(int argc, char ** argv, response_p resp) //TODO: parse string, make sure it has the correct format.
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
    if(argc!=1)
    {
        resp->success=FALSE;
        resp->error_text="Media range accepts only one parameter";
        return;
    }
    else
    {
        char ** mediaRangeCompleteList = divideMediaRangeList(argv[0]);
        char * mediaRangeComplete;
        int i=0;
        while((mediaRangeComplete=mediaRangeCompleteList[i])!=NULL)
        {
            char ** splitMediaRange = divideMediaType(mediaRangeComplete);
            if(!isValidMediaType(splitMediaRange))
            {
                resp->success=FALSE;
                resp->error_text= "found invalid mediaRange";
                recursiveDoublePointerFree(splitMediaRange);
                recursiveDoublePointerFree(mediaRangeCompleteList);
                return ;
            }
            i++;
            recursiveDoublePointerFree(splitMediaRange);
        }
        recursiveDoublePointerFree(mediaRangeCompleteList);
    }
    resp->success=TRUE;
    resp->next_argument=1;
    return;
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