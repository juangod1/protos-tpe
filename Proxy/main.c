#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/param.h>
#include "include/main.h"
#include "../Shared/include/executionValidator.h"
#include "../Shared/include/lib.h"
#include "include/proxyParse.h"

int main(int argc, char ** argv)
{
    int response = proxy_parse(argc,argv);

    file_descriptor MUA_sock = setup_MUA_socket();

    run_server(MUA_sock);

    return 0;
}

void run_server(file_descriptor MUA_sock)
{
    fd_set read_fds;
    fd_set write_fds;
    fd_set except_fds;

    const struct timespec timeout={
            .tv_sec=5, .tv_nsec=0
    };

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);

    FD_SET(MUA_sock, &read_fds);

    int select_ret;
    for(;;){
        select_ret = pselect(MUA_sock+1,&read_fds,&write_fds,&except_fds,&timeout,NULL);

        if(select_ret == -1)
        {
            perror("Poll error.");
            error();
        }
    }
}

file_descriptor setup_origin_socket(char * origin_address){
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_port = htons(ORIGIN_PORT);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    file_descriptor sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(sock < 0)
    {
        perror("Unable to create socket.\n");
        error();
    }

    if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Unable to connect to origin server.");
        error();
    }
    else{
        printf("Connected to server %s",origin_address);
    }

    return sock;
}

file_descriptor setup_MUA_socket()
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_port = htons(MUA_PORT);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    file_descriptor sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(sock < 0)
    {
        perror("Unable to create socket.");
        error();
    }
    else
    {
        printf("Listening on port %d...",MUA_PORT);
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    if(bind(sock, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
        perror("Unable to bind socket.\n");
        error();
    }

    if (listen(sock, MAXIMUM_PENDING_CONNECTIONS) < 0)
    {
        perror("Unable to listen.\n");
        error();
    }

    return sock;
}

void error()
{

}

int findMax(int * a, int size){
    int i,m=0;
    for(i=0;i<size;i++)
        m=max(m,a[i]);
    return m;
}

int max(int a, int b){
    return a>b?a:b;
}