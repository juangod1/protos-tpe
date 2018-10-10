#include <string.h>
#include "include/main.h"
#include "include/runCommandLine.h"
#include "../Shared/include/lib.h"

int main(int argc, char ** argv)
{
    int response = parser_parse(argc,argv);
    if(response==ERROR)
    {
        return ERROR;
    }
    run_parser(argv[1]);
    return 0;
}

int parser_parse(int argc, char ** argv)
{
    if(argc!=2)
    {
        printf("Please execute the program with one parameter in the following format: ./prog \"param\"\n");
        return ERROR;
    }
    if(strcmp(argv[1],"")==0)
    {
        printf("Please execute the program with one parameter in the following format: ./prog \"param\"\n");
        return ERROR;
    }
    return STANDARD;
}