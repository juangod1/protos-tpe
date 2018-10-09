#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include "include/main.h"
#include "include/options.h"
#include "../Shared/include/executionValidator.h"

int main(int argc, char ** argv)
{
    parse_command(argc,argv,initialize_options);

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
        perror("Unable to create socket.");
        error();
    }
    else
    {
        printf("Listening on port %d...",MUAPORT);
    }

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    if(bind(server, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
        perror("Unable to bind socket.");
        error();
    }

    if (listen(server, 20) < 0)
    {
        perror("Unable to listen.");
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