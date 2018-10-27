//
// Created by juangod on 08/10/18.
//

#ifndef PROTOS_TPE_MAIN_H
#define PROTOS_TPE_MAIN_H

typedef enum {
    STATS, COMMAND, EXIT
} function;

void socket_config();
int createConnection();
void requestForLogin(int fd, char* status);
void loginError(int fd, char* status);
void loginSuccess(char* connected);
char requestLoginToProxy(int fd);
void interaction();
char requestAdmin();
void adminMode();
void closeConnection(int fd);
void reportNotAvailableResource();

#endif //PROTOS_TPE_MAIN_H
