#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <asm/errno.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include "include/buffer.h"

int buffer_initialize(buffer_p * buffer,size_t size)
{
    (*buffer)=malloc(sizeof(struct buffer_t));
    (*buffer)->count=0;
    (*buffer)->size=size;
    (*buffer)->data_start=calloc(1,size);
    (*buffer)->data_ptr=(*buffer)->data_start;
}

int buffer_finalize(buffer_p buffer)
{
    if(buffer==NULL){
        return -1;
    }
    buffer->size=0;
    buffer->count=0;
    if(buffer->data_start!=NULL){
        free(buffer->data_start);
        buffer->data_start=NULL;
        buffer->data_ptr=NULL;
    }
    free(buffer);
    return 0;
}

int buffer_is_empty(buffer_p buffer){
    return buffer->count==0;
}
int buffer_read(int file_descriptor, buffer_p buffer)
{
    int characters_to_read = buffer->size-(buffer->data_ptr-buffer->data_start);
    char * read_ptr = buffer->data_ptr;

    int amount = read(file_descriptor,read_ptr,characters_to_read);
    if(amount!=-1)
    {
        buffer->count+=amount;
    }
    else if(errno==ECONNRESET){
        return amount;
    }
    else{
        perror("Read error");
    }
    return amount;
}


int buffer_indicates_parsable_message(buffer_p buffer){
    if(buffer->count<13){
        return false;
    }
    char * ptr = buffer->data_ptr;
    if(*ptr++!='+') return false;
    if(*ptr++!='O') return false;
    if(*ptr++!='K') return false;
    if(*ptr++!=' ') return false;
    if(!isdigit(*ptr++)) return false;
    while(isdigit(*ptr)){
        *ptr++;
    }
    if(*ptr++!=' ') return false;
    if(*ptr++!='o') return false;
    if(*ptr++!='c') return false;
    if(*ptr++!='t') return false;
    if(*ptr++!='e') return false;
    if(*ptr++!='t') return false;
    if(*ptr++!='s') return false;
    if(*ptr++!='\r') return false;
    if(*ptr!='\n') return false;
    return true;
}

int buffer_write(int file_descriptor, buffer_p buffer)
{
    int characters_to_write = buffer->count;
    char * write_ptr = buffer->data_ptr;

    int amount = write(file_descriptor,write_ptr,characters_to_write);
    if(amount!=characters_to_write)
    {
        perror("buffer_write: wrote less than expected");
        buffer->count-=amount;
        buffer->data_ptr+=amount;
    }
    else
    {
        buffer->count=0;
        buffer->data_ptr=buffer->data_start;
    }

    return amount;
}

int buffer_indicates_end_of_message(buffer_p buffer)
{
    if(buffer->count<2) return false;

    size_t count = buffer->count;
    char * ptr = buffer->data_ptr;
    while(count>2){
        count--;
        ptr++;
    }
    if(*ptr++!='\r') return false;
    if(*ptr!='\n') return false;
    return true;
}

int buffer_read_until_char(int file_descriptor,buffer_p buffer, char ch)
{
    int characters_to_read = buffer->size-(buffer->data_ptr-buffer->data_start);
    char * read_ptr = buffer->data_ptr;
    int amount = 0;

    while(characters_to_read>0)
    {
        int count = read(file_descriptor,read_ptr,1);
        if(count==1)
        {
            amount++;
            if(*read_ptr==ch){
                break;
            }

        }
        else if(count==0 || errno==ECONNRESET){
            break;
        }
        else{
            perror("Read error");
            break;
        }
        characters_to_read--;
        read_ptr++;
    }
    buffer->count+=amount;
    return amount;
}

int buffer_write_until_substring(int file_descriptor, buffer_p buffer, char * substring)
{
    int index=find_substring(buffer->data_ptr,buffer->count, substring);

    int characters_to_write = (index==-1)?(int)buffer->count:index;
    char * write_ptr = buffer->data_ptr;

    int amount = write(file_descriptor,write_ptr,characters_to_write);
    if(amount!=characters_to_write)
    {
        perror("buffer_write: wrote less than expected");
    }
    buffer->count-=amount;
    buffer->data_ptr+=amount;

    if(buffer->data_ptr-buffer->data_start==buffer->size){
        buffer->data_ptr=buffer->data_start;
    }

    return amount;
}

/**
 * Receives a char array buffer, its size and a null terminated substring
 * Returns the index of the last character of the first appearance of the substring in the buffer.
 * If no substring is found returns -1;
 * https://mischasan.wordpress.com/2011/03/19/building-a-better-strstr/
 */
int find_substring(char * buffer, int size, char * substring)
{
    char * init_buffer= buffer;
    char c, sc;
    size_t len;

    if ((c = *substring++) != 0) {
        len = strlen(substring);
        do {
            do {
                if(buffer-init_buffer>size-len)
                {
                    return -1;
                }
                sc = *buffer++;
            } while (sc != c);
        } while (strncmp(buffer, substring, len) != 0);
    }
    return buffer-init_buffer+len;
}

void print_buffer(buffer_p b){
    int i;

    printf("--------------------------------------------------------\n");
    for(i=0;i<b->count;i++){
        putchar(b->data_start[i]);
    }
    printf("\n--------------------------------------------------------\n");
}

int buffer_write_string(char * string, buffer_p buffer)
{
    int characters_to_write = buffer->count - 1;
    char * write_ptr = buffer->data_ptr;
    int i =0;

    while(characters_to_write>0){
        *(string+i++)=*(buffer->data_ptr);
        buffer->data_ptr++;
        characters_to_write--;
    }
    *(string+i)='\0';
    buffer->count=0;
    buffer->data_ptr=buffer->data_start;

    return characters_to_write;
}

int buffer_read_string(char * string, buffer_p buffer)
{
    int characters_to_read = buffer->size-(buffer->data_ptr-buffer->data_start);
    char * read_ptr = buffer->data_ptr;

    int count=characters_to_read;
    int i = 0;

    while(count>0 && *string!=0)
    {
        *(buffer->data_ptr+i++)=*string;
        string++;
        buffer->count++;
        count--;
    }

    return characters_to_read-count;
}

int buffer_starts_with_string(char * string,buffer_p buffer){
    char * pointer = buffer->data_ptr;
    char c= *string;
    while(c!=0)
    {
        if(c!=*pointer){
            return 0;
        }
        string++;
        pointer++;
        c=*string;
    }
    return 1;
}