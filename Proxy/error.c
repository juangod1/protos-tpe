//
// Created by juangod on 29/10/18.
//
#include "include/MasterStateMachine.h"
#include "include/list.h"
#include <stdlib.h>
#include <stdio.h>

state_machine *sm = NULL;

void error_print(char *s)
{
	printf("--------------------------------------------------------\n");
	printf(" An error has occurred. %s...\n", s);
	printf("--------------------------------------------------------\n");
}

void error_terminal()
{
	error_print("Closing proxy");
	if(sm != NULL)
	{
		disconnect_all(sm);
		free_machine(sm);
	}
	exit(-1);
}

void error_disconnect_client(state s)
{
	error_print("Disconnecting client");
	disconnect(s);
}

void init_error(state_machine *s)
{
	sm = s;
}