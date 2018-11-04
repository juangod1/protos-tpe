#include "include/proxyCommunication.h"
#include "../Shared/include/lib.h"
#include "include/options.h"
#include "include/state.h"
#include <stdbool.h>
#include <memory.h>


/*
 *  Executes Parser Process.
 *  Returns PROXY_PIPES and Parser's Pid
 */
int start_parser(char *cmd, int pipe_ret[2], state s)
{
	int pipes[NUM_PIPES][2];

	if(pipe(pipes[PROXY_READ_PIPE]) == -1)
	{
		perror("pipe");
		return ERROR;
	}
	if(pipe(pipes[PROXY_WRITE_PIPE]) == -1)
	{
		perror("pipe");
		return ERROR;
	}

	pid_t pid     = fork();
	char  *argv[] = {"parser", cmd, (get_app_context()->censored_media_types != NULL &&
	                                 strcmp(get_app_context()->censored_media_types, ""))
	                                ? get_app_context()->censored_media_types : "None",
	                 get_app_context()->replacement_message,
	                 get_app_context()->pop3filter_version, (s->user == NULL) ? "Not logged in." : s->user,
	                 (s->pass == NULL) ? "Not logged in." : s->pass};
	if(pid == 0)
	{
		close(PROXY_READ_FD);
		close(PROXY_WRITE_FD);

		dup2(PARSER_READ_FD, STDIN_FILENO);
		dup2(PARSER_WRITE_FD, STDOUT_FILENO);

		close(PARSER_READ_FD);
		close(PARSER_WRITE_FD);

		char *argv[] = {"parser", cmd, get_app_context()->censored_media_types, get_app_context()->replacement_message,
		                get_app_context()->pop3filter_version, (s->user == NULL) ? "Not logged in." : s->user,
		                (s->pass == NULL) ? "Not logged in." : s->pass, NULL};
		if(execv("parser", argv) < 0)
		{
			exit(BAD_EXIT_STATUS);
		}
	}
	else /* pid!=0; parent process */
	{
		close(PARSER_READ_FD);
		close(PARSER_WRITE_FD);

		pipe_ret[0] = PROXY_READ_FD;
		pipe_ret[1] = PROXY_WRITE_FD;

	}
	return pid;
}


int check_parser_exit_status(int parser_pid)
{
	int status;
	waitpid(parser_pid, &status, WNOHANG);

	if(WIFEXITED(status) == false || WEXITSTATUS(status) != GOOD_EXIT_STATUS)
	{
		perror("Parser failed, no content written");
		return ERROR;
	}
	return STANDARD;
}
