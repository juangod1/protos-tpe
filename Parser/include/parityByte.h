#ifndef PARITYBYTE_H
#define PARITYBYTE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


char parityByte(char * string, int size);
char * charToHex(char ch);
char hxNumberToChar(char number);
int isDigit(char c);

#endif
