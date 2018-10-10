#include "include/main.h"
#include "include/options.h"
#include "../Shared/include/executionValidator.h"

int main(int argc, char ** argv)
{
    response_p response = malloc(sizeof(response_t));
    initialize_options();
    parse_command(argc,argv,response);
    free(response);

    return 0;
}