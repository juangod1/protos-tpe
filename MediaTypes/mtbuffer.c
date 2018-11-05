#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <asm/errno.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <memory.h>

#include "../Shared/include/lib.h"
#include "../Shared/include/buffer.h"


char * mt_buffer_get_mime(buffer_p buffer)
{
  if(!buffer_starts_with_string("Content-Type: ",buffer))
  {
    return NULL;
  }
  char * ptr = buffer->data_ptr+14;
  char * str_start = ptr;
  int aux_count = 14;
  int count_start = aux_count;

  while(aux_count<=buffer->count)
  {
    if(*ptr==';' || *ptr == '\n')
    {
      break;
    }
    if(*ptr==' ' || *ptr==0)
    {
      return NULL;
    }
    aux_count++;
    ptr++;
  }
  if(*ptr!=';' && *ptr!='\n') return NULL;
  int size = aux_count-count_start;
  char * ret = calloc(1, size+1);
  memcpy(ret,str_start,size);
  return ret;
}

char * mt_buffer_get_boundary(buffer_p buffer)
{
    int index = find_substring(buffer->data_ptr,buffer->count," boundary=");
    if(index<0)
    {
        return NULL;
    }
    char * ptr = buffer->data_ptr+index;
    if(*ptr++!='"') return NULL;
    char * ret = ptr;
    int count = buffer->count-index;
    int size=0;
    while(*ptr!='"' && count>0)
    {
        size++;
        ptr++;
        count--;
    }
    if(*ptr!='"') return NULL;
    return my_strndup(ret,size);
}

void mt_buffer_discard(buffer_p buffer)
{
    buffer->count=0;
}


int mt_buffer_starts_with_boundary(char * boundary, buffer_p buffer)
{
    char * pointer = buffer->data_ptr;
    if(*(pointer++)!='-') return 0;
    if(*(pointer++)!='-') return 0;
    char c= *boundary;
    while(c!=0)
    {
        if(c!=*pointer){
            return 0;
        }
        boundary++;
        pointer++;
        c=*boundary;
    }
    return 1;
}

