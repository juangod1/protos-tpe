#ifndef PROXYPARSE_H
#define PROXYPARSE_H

#define ERROR -1
#define STANDARD 0
#define VERSION 1
#define HELP 2

int proxy_parse(int argc, char ** argv);
int execution_validator(char * arg,response_p response);


#endif