#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <wordexp.h>
#include <ctype.h>
#include <stdbool.h>
#include "include/main.h"
#include "../Shared/include/executionValidator.h"
#include "../Shared/include/lib.h"
#include "include/options.h"

#define MAX_STREAMS 64
#define MAX_BUFFER 1024

void prepareForSending(char **username, char **password);

struct sockaddr_in     addr;
static admin_context_p admin_context;


int main(int argc, char **argv)
{
	initialize_admin_context();
	int valid = parse_arguments(argc, argv);
	if(!valid)
	{
		printf("Error creating connection");
		free_struct();
		exit(EXIT_FAILURE);
	}
	admin_context = get_admin_context();
	socket_config();

	printf("Hello! Starting connection...\n");
	int fd = createConnection();

	char status = 0;
	receiveGreeting(fd);
	while(status != 2)
	{
		requestForLogin(fd, &status);
		if(status == 1)
		{
			interaction(fd);
		}
	}
	closeConnection(fd);
	return 0;
}


void socket_config()
{
	memset((void *) &addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(admin_context->port);
	addr.sin_addr.s_addr = inet_addr(admin_context->listenaddress);
}

int createConnection()
{
	int                         fd, ret;


	if((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) == -1)
	{
		printf("An error has ocurred while creating SCTP socket\n");
		perror("socket");
		closeConnection(fd);
		exit(EXIT_FAILURE);
	}

	if((ret = connect(fd, (struct sockaddr *) &addr, sizeof(addr))) == -1)
	{
		printf(("An error has ocurred while connecting the SCTP socket\n"));
		perror("connect");
		closeConnection(fd);
		exit(EXIT_FAILURE);
	}
	return fd;
}

void requestForLogin(int fd, char *status)
{
	if(requestLoginToProxy(fd))
	{
		loginSuccess(status);
	}
	else
	{
		loginError(fd, status);
	}
}

void loginError(int fd, char *status)
{
	printf("Login failed. Do you wish to retry? [Y/n]: ");
	char   *input = calloc(1, INITIAL_INPUT_SIZE);
	size_t count  = fetchLineFromStdin(&input, INITIAL_INPUT_SIZE);
	if(count != 1)
	{
		printf("Please input only Y/n");
	}
	else
	{
		if(*input == 'Y' || *input == 'y')
		{
			requestForLogin(fd, status);
		}
		else if(*input == 'N' || *input == 'n')
		{
			*status = 2;
		}
	}
	free(input);
}

void loginSuccess(char *status)
{
	printf("Login succesful\n");
	*status = 1;
}
void receiveGreeting(int fd)
{
    char buffer[MAX_BUFFER] = {0};
    sctp_recvmsg(fd, buffer, sizeof(buffer), NULL, 0, 0, 0);
    printf("%s",buffer);
}
char requestLoginToProxy(int fd)
{
	char buffer[MAX_BUFFER] = {0};
	char *usernameInput = calloc(1, INITIAL_INPUT_SIZE);
	char *passwordInput = calloc(1, INITIAL_INPUT_SIZE);

	printf("Please enter your username: ");
	fetchLineFromStdin(&usernameInput, INITIAL_INPUT_SIZE);
	printf("Please enter your password: ");
	fetchLineFromStdin(&passwordInput, INITIAL_INPUT_SIZE);
	prepareForSending(&usernameInput, &passwordInput);
	char *pos;
	if((pos = strchr(usernameInput, '\0')) != NULL)
	{
		*pos = '\n';
	}
	if((pos = strchr(passwordInput, '\0')) != NULL)
	{
		*pos = '\n';
	}
	int ret = sctp_sendmsg(fd, usernameInput, strlen(usernameInput), NULL, 0, 0, 0, 0, 0, 0);
	if(ret == -1)
	{
		printf("An error has ocurred sending USER info\n");
		perror("sctp_sendmsg");
		closeConnection(fd);
		exit(EXIT_FAILURE);
	}

	sctp_recvmsg(fd, buffer, sizeof(buffer), NULL, 0, 0, 0);

	ret = sctp_sendmsg(fd, passwordInput, strlen(passwordInput), NULL, 0, 0, 0, 0, 0, 0);
	if(ret == -1)
	{
		printf("An error has ocurred sending PASS info\n");
		perror("sctp_sendmsg");
		closeConnection(fd);
		exit(EXIT_FAILURE);
	}

	ret = sctp_recvmsg(fd, buffer, sizeof(buffer), NULL, 0, 0, 0);
    printf("%s",buffer);
	free(usernameInput);
	free(passwordInput);

	if(strncmp(buffer, "+OK", 3) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void prepareForSending(char **username, char **password)
{
	char *user = calloc(1, INITIAL_INPUT_SIZE + 6);
	strcpy(user, "USER ");
	strcat(user, *username);
	strcpy(*username, user);
	char *pass = calloc(1, INITIAL_INPUT_SIZE + 5);
	strcpy(pass, "PASS ");
	strcat(pass, *password);
	strcpy(*password, pass);
	free(user);
	free(pass);
}

void interaction(int fd)
{

	char      buffer[MAX_BUFFER];
	int       ret;
	wordexp_t p;
	char      **w;
	int       count;
	while(1)
	{
		if(fgets(buffer, MAX_BUFFER, stdin) == NULL)
		{
			closeConnection(fd);
			exit(EXIT_FAILURE);
		}
		size_t length = strlen(buffer);
		char   *pos;
		if((pos = strchr(buffer, '\n')) != NULL)
		{
			*pos = '\0';
		}

		if(strcmp(buffer, "LISTS") == 0)
		{
			char *pos;
			if((pos = strchr(buffer, '\0')) != NULL)
			{
				*pos = '\n';
			}
			ret = sctp_sendmsg(fd, (void *) buffer, length, NULL, 0, 0, 0, 0, 0, 0);
            printResponse(fd);
		}
		else if(strncmp(buffer, "STATS", 5) == 0)
		{
			int response = wordexp(buffer, &p, 0);
			if(response != 0)
			{
				printf("There seems to be an internal error\n");
			}
			else
			{
				count = (int) p.we_wordc;
				w     = p.we_wordv;

				if(count != 2 || !isdigit(w[1][0]) || w[1][1] != '\0')
				{
					printf("Usage: STATS number, number must be a digit\n");
				}
				else
				{
					char *pos;
					if((pos = strchr(buffer, '\0')) != NULL)
					{
						*pos = '\n';
					}
					ret = sctp_sendmsg(fd, (void *) buffer, length, NULL, 0, 0, 0, 0, 0, 0);
					printResponse(fd);
				}
			}
			wordfree(&p);
		}
		else if(strncmp(buffer, "ACTIVE", 6) == 0)
		{
			int response = wordexp(buffer, &p, 0);
			if(response != 0)
			{
				printf("There seems to be an internal error\n");
			}
			else
			{
				count = (int) p.we_wordc;
				w     = p.we_wordv;
				if(count == 1)
				{
					char *pos;
					if((pos = strchr(buffer, '\0')) != NULL)
					{
						*pos = '\n';
					}
					ret = sctp_sendmsg(fd, (void *) buffer, length, NULL, 0, 0, 0, 0, 0, 0);
					printResponse(fd);
				}
				else if(count == 2)
				{
					if(strcmp(w[1], "1") == 0 || strcmp(w[1], "0") == 0)
					{
						char *pos;
						if((pos = strchr(buffer, '\0')) != NULL)
						{
							*pos = '\n';
						}
						ret = sctp_sendmsg(fd, (void *) buffer, length, NULL, 0, 0, 0, 0, 0, 0);
						printResponse(fd);
					}
					else
					{
						printf("Usage: ACTIVE [BOOL], bool must be 0 or 1");
					}
				}
				else
				{
					printf("Usage ACTIVE [BOOL]");
				}
			}

			wordfree(&p);
		}
		else if(strncmp(buffer, "FILTER", 6) == 0)
		{
			int response = wordexp(buffer, &p, 0);
			if(response != 0)
			{
				printf("There seems to be an internal error\n");
			}
			else
			{
				count = (int) p.we_wordc;
				w     = p.we_wordv;
                char *pos;
                if(count == 1 || count == 2 || (pos = strchr(buffer,'@')) != NULL)
				{
					if((pos = strchr(buffer, '\0')) != NULL)
					{
						*pos = '\n';
					}
					ret = sctp_sendmsg(fd, (void *) buffer, length, NULL, 0, 0, 0, 0, 0, 0);
					printResponse(fd);
				}

			}
			wordfree(&p);
		}
		else if(strcmp(buffer, "HELP") == 0)
		{
			printf("LISTS---------------- Lists the possible monitoring options along with its corresponding number\n");
			printf("STATS number--------- Returns the monitoring information corresponding to number (number between 0 and 9)\n");
			printf("ACTIVE [BOOL]-------- With no BOOL, returns the current transformation filter's status\n"
			       "                      With BOOL, activates (1) or deactivates (0) the current filter\n");
			printf("FILTER [CMD]--------- With no CMD, returns current transformation filter\n"
			       "                      With CMD (a command name compatible with system(3)) changes the current tranformation filter into CMD\n"
                   "                      @ escapes all blank spaces after it\n");
			printf("QUIT----------------- Closes the connection\n");
			printf("HELP----------------- Lists the possible input options\n");


		}
		else if(strcmp(buffer, "QUIT") == 0)
		{
			char *pos;
			if((pos = strchr(buffer, '\0')) != NULL)
			{
				*pos = '\n';
			}
			ret = sctp_sendmsg(fd, (void *) buffer, length, NULL, 0, 0, 0, 0, 0, 0);
			printResponse(fd);
			closeConnection(fd);
            exit(EXIT_SUCCESS);

		}
		else
		{
			printf("Incorrect input, try HELP for options\n");
			ret = -2;
		}

		if(ret == -1)
		{
			printf("An error has ocurred sending the message\n");
			closeConnection(fd);
			exit(EXIT_FAILURE);
		}
	}
}
void printResponse(int fd)
{
    char responseBuffer[MAX_BUFFER] = {0};

    int flag = 0;

    while(!flag)
    {
        sctp_recvmsg(fd, responseBuffer, sizeof(responseBuffer), NULL, 0, 0, 0);
        if(charbuffer_ends_with_doubleCRLF(responseBuffer))
        {
            flag = 1;
        }
        printf("%s",responseBuffer);
        clean_buffer(responseBuffer);
    }

}
int charbuffer_ends_with_doubleCRLF(char* buffer)
{
    char* s = buffer;
    while(*s != '\0')
    {
        if(*s=='\r')
        {
            s++;
            if(*s == '\n')
            {
                s++;
                if(*s == '\r')
                {
                    s++;
                    if(*s == '\n')
                    {
                        s++;
                        if(*s == '\0')
                        {
                            return true;
                        }
                    }
                }
            }
        }
        else
        {
            s++;
        }
    }
    return false;
}
void clean_buffer(char* buffer)
{
    char* s = buffer;
    while(*s != '\0')
    {
        *s = '\0';
        *s++;
    }
}
void closeConnection(int fd)
{
	printf("Goodbye, hope to see you soon!\n");
	free_struct();
	close(fd);
}
