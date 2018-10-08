//
// Created by juangod on 08/10/18.
//

#include "include/executionValidator.h"


void initialize_options()
{
    option_register("Test Mode","t",test_mode,"A mode that tests.");
    option_register("Non test Mode","n",non_test_mode,"A mode that doesn't test");
}

void test_mode()
{
    printf("This is test mode!\n");fflush(stdout);
}

void non_test_mode()
{
    printf("This is non test mode!\n");fflush(stdout);
}