#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/param.h>
#include "include/main.h"
#include "../Shared/include/executionValidator.h"
#include "include/proxyParse.h"

int main(int argc, char ** argv)
{
    int response = proxy_parse(argc,argv);

    file_descriptor MUA_sock = setup_MUA_socket();
    file_descriptor Origin_sock = setup_origin_socket("127.0.0.1");

    //run_server(MUA_sock, Origin_sock);

    return 0;
}

/*void run_server(file_descriptor MUA_sock, file_descriptor Origin_sock)
{
    fd_set file_descriptors;
    struct timeval tv;
    int select_ret;

    FD_ZERO(&file_descriptors);
    FD_SET(MUA_sock,&file_descriptors);
    FD_SET(MUA_sock,&file_descriptors);

    file_descriptors[0].fd = MUA_sock;
    file_descriptors[1].fd = Origin_sock;

    int max_fd = findMax((int*)file_descriptors,SELECT_FILE_DESCRIPTOR_AMOUNT);

    for(;;){
        select_ret = pselect(max_fd,);

        if(select_ret == -1)
        {
            perror("Poll error.");
            error();
        }
    }
}*/

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

    if (listen(sock, 20) < 0)
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