#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <asm/errno.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include "include/buffer.h"
#include "../Proxy/include/error.h"

int buffer_initialize(buffer_p *buffer, size_t size)
{
	(*buffer) = malloc(sizeof(struct buffer_t));
	(*buffer)->count      = 0;
	(*buffer)->size       = size;
	(*buffer)->data_start = calloc(1, size);
	(*buffer)->data_ptr   = (*buffer)->data_start;
}

int buffer_finalize(buffer_p buffer)
{
	if(buffer == NULL)
	{
		return -1;
	}
	buffer->size  = 0;
	buffer->count = 0;
	if(buffer->data_start != NULL)
	{
		free(buffer->data_start);
		buffer->data_start = NULL;
		buffer->data_ptr   = NULL;
	}
	free(buffer);
	return 0;
}

int buffer_is_empty(buffer_p buffer)
{
	return buffer->count == 0;
}

int buffer_read(int file_descriptor, buffer_p buffer)
{
	int  characters_to_read = buffer->size - (buffer->data_ptr - buffer->data_start);
	char *read_ptr          = buffer->data_ptr;

	int amount = read(file_descriptor, read_ptr, characters_to_read);
	if(amount != -1)
	{
		buffer->count += amount;
	}
	else if(errno == ECONNRESET)
	{
		return amount;
	}
	else
	{
		perror("Read error");
	}
	return amount;
}


int buffer_indicates_parsable_message(buffer_p buffer)
{
	char *ptr = buffer->data_ptr;
	int amount = buffer->count;

	if(*ptr == '\n' || *ptr++ == ':'  || amount--<=0)
	{ return false; }
	while(*ptr++ != ':')
	{
		if( amount--<=0 || *ptr=='\n') return false;
	}
	return true;
}

int buffer_write(int file_descriptor, buffer_p buffer)
{
	int  characters_to_write = buffer->count;
	char *write_ptr          = buffer->data_ptr;

	int amount = write(file_descriptor, write_ptr, characters_to_write);
	if(amount != characters_to_write)
	{
		perror("buffer_write: wrote less than expected");
		buffer->count -= amount;
		buffer->data_ptr += amount;
	}
	else
	{
		buffer->count    = 0;
		buffer->data_ptr = buffer->data_start;
	}

	return amount;
}

int buffer_ends_with_string(buffer_p buffer, char * string)
{
    if(string==NULL)
        return false;
	size_t size = strlen(string);
	if(size==0) return true;
	if(size>buffer->count) return false;

	char * ptr = buffer->data_ptr + (buffer->count-size);

	while(*string!=0)
	{
		if(*string!=*ptr)
		{
			return false;
		}
		ptr++;
		string++;
	}
	return true;

}

int buffer_indicates_end_of_message(buffer_p buffer)
{
	return buffer_ends_with_string(buffer,"\r\n");
}
int buffer_read_until_string(int file_descriptor, buffer_p buffer, char * str) //\r\n\0 || \r\n\r\n\0
{
	int read_index=0, write_index=0, circular_buffer_size=0;
	size_t size = strlen(str);
	if(size==0){
		return 0;
	}
	char circular_buffer[size];
	memset(circular_buffer,0,size);

	int  characters_to_read = buffer->size - (buffer->data_ptr - buffer->data_start);
	char *read_ptr          = buffer->data_ptr;
	int  amount             = 0;

	while(characters_to_read>amount)
	{
		int count = read(file_descriptor, read_ptr, 1);
		if(count == 1)
		{
			amount++;
		}
		else if(count == 0 || errno == ECONNRESET)
		{
			break;
		}
		else
		{
			perror("Read error");
			break;
		}
		char ch=*read_ptr++;

		circular_buffer[write_index++]=ch;
		write_index=write_index%size;

		if(circular_buffer_size==size)
		{
			read_index=(read_index+1)%size;
			int i=0;
			while(i>=0)
			{
				if(*(str+i)==0){
					buffer->count+=amount;
					return amount;
				}
				int read_from = (read_index+i)%size;
				if(*(str+i)!=circular_buffer[read_from]){
					i=-1;
				}else{
					i++;
				}
			}

		}
		else
		{
			circular_buffer_size++;
		}
	}
	buffer->count+=amount;
	return amount;


}

int buffer_read_until_char(int file_descriptor, buffer_p buffer, char ch)
{
	int  characters_to_read = buffer->size - (buffer->data_ptr - buffer->data_start);
	char *read_ptr          = buffer->data_ptr;
	int  amount             = 0;

	while(characters_to_read > 0)
	{
		int count = read(file_descriptor, read_ptr, 1);
		if(count == 1)
		{
			amount++;
			if(*read_ptr == ch)
			{
				break;
			}

		}
		else if(count == 0 || errno == ECONNRESET)
		{
			break;
		}
		else
		{
			perror("Read error");
			break;
		}
		characters_to_read--;
		read_ptr++;
	}
	buffer->count += amount;
	return amount;
}

int buffer_write_until_substring(int file_descriptor, buffer_p buffer, char *substring)
{
	int index = find_substring(buffer->data_ptr, buffer->count, substring);

	int  characters_to_write = (index == -1) ? (int) buffer->count : index;
	char *write_ptr          = buffer->data_ptr;

	int amount = write(file_descriptor, write_ptr, characters_to_write);
	if(amount != characters_to_write)
	{
		perror("buffer_write: wrote less than expected");
	}
	buffer->count -= amount;
	buffer->data_ptr += amount;

	if(buffer->data_ptr - buffer->data_start == buffer->size)
	{
		buffer->data_ptr = buffer->data_start;
	}

	return amount;
}

/**
 * Receives a char array buffer, its size and a null terminated substring
 * Returns the index of the last character of the first appearance of the substring in the buffer.
 * If no substring is found returns -1;
 * https://mischasan.wordpress.com/2011/03/19/building-a-better-strstr/
 */
int find_substring(char *buffer, int size, char *substring)
{
	char   *init_buffer = buffer;
	char   c, sc;
	size_t len;

	if((c = *substring++) != 0)
	{
		len = strlen(substring);
		do
		{
			do
			{
				if(buffer - init_buffer > size - len)
				{
					return -1;
				}
				sc = *buffer++;
			} while(sc != c);
		} while(strncmp(buffer, substring, len) != 0);
	}
	return buffer - init_buffer + len;
}

void print_buffer(buffer_p b)
{
	int i;

	printf("--------------------------------------------------------\n");
	for(i = 0; i < b->count; i++)
	{
		putchar(b->data_start[i]);
	}
	printf("\n--------------------------------------------------------\n");
}

int buffer_write_string(char *string, buffer_p buffer)
{
	int  characters_to_write = buffer->count - 1;
	char *write_ptr          = buffer->data_ptr;
	int  i                   = 0;

	while(characters_to_write > 0)
	{
		*(string + i++) = *(buffer->data_ptr);
		buffer->data_ptr++;
		characters_to_write--;
	}
	*(string + i)            = '\0';
	buffer->count    = 0;
	buffer->data_ptr = buffer->data_start;

	return characters_to_write;
}

int buffer_read_string(char *string, buffer_p buffer)
{
	int  characters_to_read = buffer->size - (buffer->data_ptr - buffer->data_start);
	char *read_ptr          = buffer->data_ptr;

	int count = 0;
	int i     = 0;

	while(characters_to_read > 0 && *string != 0)
	{
		*(buffer->data_ptr + i++) = *string;
		string++;
		buffer->count++;
		characters_to_read--;
		count++;
	}

	return count;
}

int buffer_read_string_endline(char* string, buffer_p buffer, int type){
	int  characters_to_read = buffer->size - (buffer->data_ptr - buffer->data_start);
	switch(type){
		case 0:
			//Linea normal
			characters_to_read -= 4;
			break;
		case 1:
			//Linea 1 de multi
			characters_to_read -= 2;
			break;
		case 2:
			//Linea final multilinea
			characters_to_read -= 5;
			break;
		default:
			break;
	}
	char *read_ptr          = buffer->data_ptr;

	int count = 0;
	int i     = 0;

	while(characters_to_read > 0 && *string != 0)
	{
		*(buffer->data_ptr + i++) = *string;
		string++;
		buffer->count++;
		characters_to_read--;
		count++;
	}
	switch(type){
		case 0:
			memcpy((buffer->data_ptr + i++),"\r\n\r\n",4);
			buffer->count += 4;
			break;
		case 1:
			memcpy((buffer->data_ptr + i++),"\r\n",2);
			buffer->count += 2;
			break;
		case 2:
			memcpy((buffer->data_ptr + i++),"\r\n.\r\n",5);
			buffer->count += 5;
			break;
		default:
			break;
	}
	return count;
}

int buffer_starts_with_string(char *string, buffer_p buffer)
{
	char *pointer = buffer->data_ptr;
	char c        = *string;
	while(c != 0)
	{
		if(c != *pointer)
		{
			return false;
		}
		string++;
		pointer++;
		c = *string;
	}
	return true;
}