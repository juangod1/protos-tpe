//
// Created by juangod on 08/10/18.
//

#ifndef PROTOS_TPE_MAIN_H
#define PROTOS_TPE_MAIN_H

void waitForConection();
void requestForLogin();
void loginError();
void loginSuccess(char* connected);
char requestLoginToProxy();
void interactionMode();

#endif //PROTOS_TPE_MAIN_H
