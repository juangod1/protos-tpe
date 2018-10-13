#include "include/lib.h"

int fetchInputFromStdin(char ** bufferPosition, size_t size)
{
    return fetchInputFromFile(bufferPosition, stdin, size);
}

int fetchInputFromFile(char ** bufferPosition, FILE * f, size_t  size)
{
    int c;
    int counter=0;
    while((c=fgetc(f))!=EOF){
        if(counter>=size){
            size+=INITIAL_INPUT_SIZE;
            (*bufferPosition) = realloc((*bufferPosition), size);
        }
        *((*bufferPosition)+counter)=(char)c;
        counter++;
    }
    return counter;
}
