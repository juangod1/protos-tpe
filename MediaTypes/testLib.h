//
// Created by juangod on 18/04/18.
//

#ifndef TP2_TEMPORARY_TESTLIB_H
#define TP2_TEMPORARY_TESTLIB_H

#include <stdio.h>
#include <stdlib.h>



void checkIsNotNull(void * pointer);
void ok();
void fail(char * errorMsg);
void notImplemented();
void thenSuccess();
void checkStringsEqual(char* str1, char* str2);
void givenNothing();
#endif //TP2_TEMPORARY_TESTLIB_H
