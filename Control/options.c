
#include <getopt.h>
#include <errno.h>
#include <inttypes.h>
#include "../Shared/include/executionValidator.h"
#include "include/options.h"

static admin_context_p admin_context = NULL;

admin_context_p get_admin_context()
{
    return admin_context;
}

void initialize_app_context()
{
    admin_context=malloc(sizeof(app_context_t));
    admin_context->port = 9090;
    admin_context->listenaddress = malloc(strlen("127.0.0.1"));
    memcpy(admin_context->listenaddress,"127.0.0.1",strlen("127.0.0.1"));
}

int parse_arguments(int argc, char ** argv)
{
    int c;
    opterr = 0;
    int valid;
    char buf[7];
    while ((c = getopt(argc,argv,"l:p:")) != -1)
    {
        switch (c)
        {
            case 'p':
                valid = connection_port_validation((uint16_t) optarg);
                if(valid)
                {
                    admin_context->port = connection_port(optarg);
                }
                else
                {
                    printf("Invalid port\n");
                }
                break;
            case 'l':
                valid = listen_address_validation(optarg);
                if(valid)
                {
                    memcpy(admin_context->listenaddress,optarg, strlen(optarg));
                }
                else
                {
                    printf("Invalid address\n");
                }
                break;

            case '?':
                if(optopt == 'p' || optopt == 'l')
                {
                    printf("Option -%c requires an argument\n",optopt);
                } else
                {
                    printf("Invalid arguments\n");
                }
                exit(1);
            default:
                break;
        }
    }
    return 1;
}

uint16_t connection_port(char* port)
{
    char* end = 0;
    errno = 0;
    intmax_t val = strtoimax(port,&end,10);
    if(errno==ERANGE || val < 0 || val > UINT16_MAX || end == port || *end != '\0')
    {
        exit(1);
    }
    return val;
}
int connection_port_validation(uint16_t port)
{
    return 1;
}
int listen_address_validation(char* address)
{
    return 1;
}

void free_struct()
{
    free(admin_context->listenaddress);
    free(admin_context);
}