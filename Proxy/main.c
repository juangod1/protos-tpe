#include "include/main.h"
#include "include/options.h"
#include "../Shared/include/executionValidator.h"

int main(int argc, char ** argv)
{
    parse_command(argc,argv,initialize_options);

    return 0;
}