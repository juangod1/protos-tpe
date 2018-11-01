//
// Created by juangod on 18/04/18.
//
#include "testLib.h"


void thenSuccess()
{
    ok();
}

void ok()
{
    printf("    Ok!\n");
}

void fail(char * errorMsg)
{
    printf("    Failed\n");
    printf("%s\n",errorMsg );
}
