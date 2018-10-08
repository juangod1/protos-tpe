#include "include/main.h"

int main(int argc, char ** argv)
{
    parse_command(argc,argv);

    return -1;
}

int parse_command(int argc, char ** argv){
    initialize_options();
    int response = validate_arguments(argc, argv);

    if(response<0)
    {
        return -1;
    }
    execute_options();
    return 0;
}