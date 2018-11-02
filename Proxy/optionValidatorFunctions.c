#include <sys/stat.h>
#include <arpa/inet.h>
#include "include/optionValidatorFunctions.h"
#include "../Shared/include/lib.h"

response_p
validate_server_string(char *string, response_p response) //TODO: parse string, make sure it has the correct format.
{
	if(strcmp(string, "-h") == 0 || strcmp(string, "-v") == 0)
	{
		response->success    = FALSE;
		response->error_text = "Invalid server address";
		return response;
	}
	response->success       = TRUE;
	response->next_argument = 0;
	return response;
}

void pop3_direction_validation(int argc, char **argv,
                               response_p resp)//TODO: parse string, make sure it has the correct format.
{
    if(argc<1 || argv[0] == NULL)
    {
        resp->success    = FALSE;
        resp->error_text = "Pop3 proxy address accepts only one parameter";
        return;
    }
    else
    {
        char buf[16];

        if(inet_pton(AF_INET, argv[0], buf) || inet_pton(AF_INET6, argv[0], buf))
        {
            resp->success       = TRUE;
            resp->next_argument = 1;
        }
        else
        {
            resp->success    = FALSE;
            resp->error_text = "Invalid pop3 proxy address";
            return;
        }
    }
}

void error_specification_validation(int argc, char **argv,
                                    response_p resp) //TODO: parse string, make sure it has the correct format.
{//tiene que ser un path valido
    if(argc<1 || argv[0] == NULL)
    {
        resp->success    = FALSE;
        resp->error_text = "Error specification accepts only one parameter";
        return;
    }
    else
    {
        struct stat statbuf;
        int i = stat(argv[0], &statbuf);
        if(i == -1)
        {
            resp->success    = FALSE;
            resp->error_text = "Invalid error path";
            return;
        }
    }

	resp->success       = TRUE;
	resp->next_argument = 1;
}

void management_direction_validation(int argc, char **argv,
                                     response_p resp) //TODO: parse string, make sure it has the correct format.
{//tiene que ser una direccion ipv4 o ipv6 valida
    if(argc<1 || argv[0] == NULL)
    {
        resp->success    = FALSE;
        resp->error_text = "Management address accepts only one parameter";
        return;
    }
    else
    {
        char buf[16];

        if(inet_pton(AF_INET, argv[0], buf) || inet_pton(AF_INET6, argv[0], buf))
        {
            resp->success       = TRUE;
            resp->next_argument = 1;
        }
        else
        {
            resp->success    = FALSE;
            resp->error_text = "Invalid management address";
            return;
        }
    }
}

void replacement_message_validation(int argc, char **argv, response_p resp)
{
    if(argc<1 || argv[0] == NULL)
    {
        resp->success    = FALSE;
        resp->error_text = "Replacement message accepts only one parameter";
        return;
    }
	resp->success       = TRUE;
	resp->next_argument = 1;
}

void command_specification_validation(int argc, char **argv,
                                      response_p resp) //TODO: parse string, make sure it has the correct format.
{
    if(argc<1 || argv[0] == NULL)
    {
        resp->success    = FALSE;
        resp->error_text = "Command specification accepts only one parameter";
        return;
    }
	resp->success       = TRUE;
	resp->next_argument = 1;
}

void censored_mediatype_validation(int argc, char **argv, response_p resp)
{
	if(argc < 1)
	{
		resp->success    = FALSE;
		resp->error_text = "Media range accepts only one parameter";
		return;
	}
	else
	{
		char **mediaRangeCompleteList = divideMediaRangeList(argv[0]);
		char *mediaRangeComplete;
		int  i                        = 0;
		while((mediaRangeComplete = mediaRangeCompleteList[i]) != NULL)
		{
			char **splitMediaRange = divideMediaType(mediaRangeComplete);
			if(!isValidMediaType(splitMediaRange))
			{
				resp->success    = FALSE;
				resp->error_text = "found invalid mediaRange";
				recursiveDoublePointerFree(splitMediaRange);
				recursiveDoublePointerFree(mediaRangeCompleteList);
				return;
			}
			i++;
			recursiveDoublePointerFree(splitMediaRange);
		}
		recursiveDoublePointerFree(mediaRangeCompleteList);
	}
	resp->success       = TRUE;
	resp->next_argument = 1;
	return;
}

void management_port_validation(int argc, char **argv, response_p resp)
{
	if(argc < 1 || argv[0] == NULL)
	{
		resp->success    = FALSE;
		resp->error_text = "Management Port accepts only one parameter";
		return;
	}
	is_valid_port(argv[0], resp);
}

void local_port_validation(int argc, char **argv, response_p resp)
{
	if(argc < 1 || argv[0] == NULL)
	{
		resp->success    = FALSE;
		resp->error_text = "Local Port accepts only one parameter";
		return;
	}
	is_valid_port(argv[0], resp);
}

void origin_port_validation(int argc, char **argv, response_p resp)
{
	if(argc < 1 || argv[0] == NULL)
	{
		resp->success    = FALSE;
		resp->error_text = "Origin Port accepts only one parameter";
		return;
	}
	is_valid_port(argv[0], resp);
}

void is_valid_port(char *port_string, response_p resp)
{
	char *endptr        = port_string;
	long port           = strtol(port_string, &endptr, BASE_TEN);
	if(*endptr != 0)
	{
		resp->success    = FALSE;
		resp->error_text = "Invalid Port number";
		return;
	}
	if(port > MAX_PORT_NUMBER || port < MIN_PORT_NUMBER)
	{
		resp->success    = FALSE;
		resp->error_text = "Port number must be in valid port-range";
		return;
	}
	resp->success       = TRUE;
	resp->next_argument = 1;
}
