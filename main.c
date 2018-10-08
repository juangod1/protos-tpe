#include "include/main.h"

int main(int argc, char ** argv)
{
    initialize_options();
    validation_response_t response = validate_arguments(argc, argv);

    if(response.length>0)
    {
        execute_options(response.selectedOptions, response.length);
        return 0;
    }
    return -1;
}

void test_mode()
{
    printf("This is test mode!\n");fflush(stdout);
}

void non_test_mode()
{
    printf("This is non test mode!\n");fflush(stdout);
}