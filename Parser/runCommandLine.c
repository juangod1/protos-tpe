#include <stdbool.h>
#include "include/runCommandLine.h"
#include "../Shared/include/lib.h"

//https://stackoverflow.com/questions/28507950/calling-ls-with-execv
//https://jineshkj.wordpress.com/2006/12/22/how-to-capture-stdin-stdout-and-stderr-of-child-program/

int run_parser(char *command)
{
	int status = system(command);
	if(WSTOPSIG(status) == 127) //todo: man says I shouldn't
	{
		perror("Invalid command");
		exit(BAD_EXIT_STATUS);
	}
	exit(GOOD_EXIT_STATUS);
}