#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "include/main.h"

static option_t options[MAX_OPTIONS];
static options_size = 0;

int main(int argc, char ** argv) {

    initialize_options();

    int count;
    for(count=0;count<argc-1;count++){
        if(argv[argc-1-count][0]=='-'){
            argument_validator(argv[argc-1-count][1]);
        }
    }
    return 0;
}

void initialize_options(){
    option_register("Test Mode","t",test_mode,"A mode that tests.");
    option_register("Non test Mode","n",non_test_mode,"A mode that doesen't test");
}

void option_register(char * name, char * command, void (*function)(), char * description){
    option_t option = {};
    option.name = name;
    option.command = command;
    option.function = function;
    option.description = description;
    options[options_size++]=option;
}

int argument_validator(char arg){
    int i;
    for(i=0;i<options_size;i++){
        if(arg == options[i].command[0]){
            void (*execute_option)() = options[i].function;
            execute_option();
        }
    }
}

void test_mode(){
    printf("This is test mode!");fflush(stdout);
}

void non_test_mode(){
    printf("This is non test mode!");fflush(stdout);
}