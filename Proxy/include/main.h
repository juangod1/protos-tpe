#ifndef PROTOS_TPE_MAIN_H
#define PROTOS_TPE_MAIN_H

typedef int file_descriptor;


#define ORIGIN_PORT  110
#define SELECT_FILE_DESCRIPTOR_AMOUNT 2
#define MAXIMUM_PENDING_CONNECTIONS 50

file_descriptor setup_MUA_socket();

file_descriptor setup_origin_socket();

void run_server();

int max(int a, int b);

int findMax(int *a, int size);

int proxy_main();

#endif //PROTOS_TPE_MAIN_H
