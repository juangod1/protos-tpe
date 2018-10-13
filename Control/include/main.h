//
// Created by juangod on 08/10/18.
//

#ifndef PROTOS_TPE_MAIN_H
#define PROTOS_TPE_MAIN_H

typedef enum {
    STATS, COMMAND, EXIT
} function;

void waitForConection();
void requestForLogin();
void loginError();
void loginSuccess(char* connected);
char requestLoginToProxy();
void interaction();
char requestAdmin();
void adminMode();
void closeConection();
void reportNotAvailableResource();

#endif //PROTOS_TPE_MAIN_H
