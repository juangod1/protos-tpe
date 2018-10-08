#include "include/main.h"

int main(int argc, char ** argv)
{
    parse_command(argc,argv);

    return -1;
}

int parse_command(int argc, char ** argv){
    initialize_options();
    validation_response_t response = validate_arguments(argc, argv);

    if(response.length>0)
    {
        execute_options(response.selectedOptions, response.length);
        return 0;
    }
    return -1;
}