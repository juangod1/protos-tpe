#ifndef PROTOS_TPE_MAIN_H
#define PROTOS_TPE_MAIN_H

#define MUAPORT 1110

void setup_MUA_socket();
void error();
void run_server();

int proxy_main();
#endif //PROTOS_TPE_MAIN_H
