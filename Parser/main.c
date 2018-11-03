#include <string.h>
#include <stdbool.h>
#include "include/main.h"
#include "include/runCommandLine.h"
#include "../Shared/include/lib.h"

int main(int argc, char **argv)
{
	int response = parser_parse(argc, argv);
	if(response == ERROR)
	{
		exit(BAD_EXIT_STATUS);
	}

	setenv("FILTER_MEDIAS", argv[2], true);
	setenv("FILTER_MSG", argv[3], true);
	setenv("POP3FILTER_VERSION", argv[4], true);
	setenv("POP3_USERNAME", argv[5], true);
	setenv("POP3_SERVER", argv[6], true);

	if(strcmp(argv[1],"mediatypes")==0)
	{
		char * args[1];
		args[0] = NULL;
		execv("mediatypes",args);
	}

	response = run_parser(argv[1]);
	if(response == ERROR)
	{
		exit(BAD_EXIT_STATUS);
	}
	exit(GOOD_EXIT_STATUS);
}

int parser_parse(int argc, char **argv)
{
	// Arguments: command FILTER_MEDIAS FILTER_MSG POP3FILTER_VERSION POP3_USERNAME POP3_SERVER
	if(argc != 7)
	{
		perror("INCORRECT PARAMETERS FOR PARSER\n");
		return ERROR;
	}

	return STANDARD;
}
