#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include "include/main.h"
#include "../Shared/include/executionValidator.h"
#include "include/proxyParse.h"

int main(int argc, char ** argv)
{
    int response = proxy_parse(argc,argv);

    if(response==ERROR)
    {
        printf("Program execution stopped.\n");
    }
    if(response==STANDARD)
    {
        execute_options();
        proxy_main();
    }
    if(response==HELP)
    {
        help();
    }
    if(response==VERSION)
    {
        version();
    }
    return response;
}

int proxy_main()
{
    printf("Starting Program...\n");
    setup_MUA_socket();

    return 0;
}

void setup_MUA_socket()
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_port = htons(MUAPORT);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    const int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(server < 0)
    {
        perror("Unable to create socket.\n");
        error();
    }
    else
    {
        printf("Listening on port %d...\n",MUAPORT);
    }

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    if(bind(server, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
        perror("Unable to bind socket.\n");
        error();
    }

    if (listen(server, 20) < 0)
    {
        perror("Unable to listen.\n");
        error();
    }

    run_server();
}

void error()
{

}

void run_server()
{

}