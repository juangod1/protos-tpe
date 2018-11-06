#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <asm/errno.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <poll.h>
#include "include/buffer.h"
#include "../Proxy/include/error.h"

int buffer_big_read(int file_descriptor, const struct buffer_t *buffer);

int buffer_initialize(buffer_p *buffer, size_t size, size_t big_buffer_size)
{
	(*buffer) = malloc(sizeof(struct buffer_t));
	(*buffer)->count      = 0;
	(*buffer)->size       = size;
	(*buffer)->data_start = calloc(1, size);
	(*buffer)->data_ptr   = (*buffer)->data_start;
	(*buffer)->big_buffer = malloc(sizeof(struct big_buffer_t));
	(*buffer)->big_buffer->data_start = malloc(big_buffer_size);
	(*buffer)->big_buffer->size = big_buffer_size;
	(*buffer)->big_buffer->write_index = 0;
	(*buffer)->big_buffer->read_size = 0;
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
	if(buffer->big_buffer !=NULL)
	{
		if(buffer->big_buffer->data_start != NULL)
		{
			free(buffer->big_buffer->data_start);
			buffer->big_buffer->data_start=NULL;
		}
		buffer->big_buffer->size=0;
		buffer->big_buffer->write_index=0;
		buffer->big_buffer->read_size=0;
		free(buffer->big_buffer);
	}
	free(buffer);
	return 0;
}

int buffer_is_empty(buffer_p buffer)
{
	return buffer->count==0;
}

int buffer_big_is_empty(buffer_p buffer)
{
	return buffer->big_buffer->read_size==buffer->big_buffer->write_index;
}

int buffer_read(int file_descriptor, buffer_p buffer)
{
	int ret = buffer_fill_until_string(buffer,"\n");
	if(!ret)
	{
		buffer_big_read(file_descriptor, buffer);
		ret = buffer_fill_until_string(buffer,"\n");
	}
	return ret;
}

int buffer_big_read(int file_descriptor, const struct buffer_t *buffer) {
	size_t  characters_to_read = buffer->big_buffer->size;
	char *read_ptr          = buffer->big_buffer->data_start;

	int amount = read(file_descriptor, read_ptr, characters_to_read);
	if(amount != -1)
	{
		buffer->big_buffer->read_size = amount;
	}
	else if(errno == ECONNRESET)
	{
		return amount;
	}
	else
	{
		perror("Read error");
	}
	buffer->big_buffer->write_index=0;
	buffer->big_buffer->read_size=amount;
	return amount;
}

int buffer_fill_until_string(buffer_p buffer, char * str)
{
	if(buffer_big_is_empty(buffer)) return false;
	int ret=true;
	char * read_ptr = buffer->big_buffer->data_start + buffer->big_buffer->write_index;
	int characters_to_read = buffer->big_buffer->size - buffer->big_buffer->write_index;
	int characters_to_copy = find_substring(read_ptr,characters_to_read,str);
	if(characters_to_copy==-1)
	{
		characters_to_copy = buffer->big_buffer->size - buffer->big_buffer->write_index;
		ret=false;
	}
	int available_size = buffer->size-buffer->count;
	if(available_size<characters_to_copy)
	{
		characters_to_copy=available_size;
	}
	if(characters_to_copy==77)
	{
		int breakpoint;
		breakpoint=1;
	}
	memcpy(buffer->data_ptr,read_ptr,characters_to_copy);
	buffer->big_buffer->write_index+=characters_to_copy;
	buffer->count+=characters_to_copy;
	buffer->data_ptr+=characters_to_copy;
	return ret;
}


int buffer_indicates_parsable_message(buffer_p buffer)
{
	char *ptr   = buffer->data_start;
	int  amount = buffer->count;

	if(*ptr == '\n' || *ptr++ == ':' || amount-- <= 0)
	{ return false; }
	while(*ptr++ != ':')
	{
		if(amount-- <= 0 || *ptr == '\n')
		{ return false; }
	}
	return true;
}

int buffer_write_after_index(int file_descriptor, buffer_p buffer, int index)
{
	int  characters_to_write = buffer->count - index;
	char *write_ptr          = buffer->data_start + index;

	if(characters_to_write < 1)
	{
		return 0;
	}

	int amount = write(file_descriptor, write_ptr, characters_to_write);
	if(amount != characters_to_write)
	{
		perror("buffer_write: wrote less than expected");
	}
	buffer->count    = 0;
	buffer->data_ptr = buffer->data_start;

	return amount;
}


int buffer_write(int file_descriptor, buffer_p buffer)
{
	int  characters_to_write = buffer->count;
	char *write_ptr          = buffer->data_start;

	int amount = write(file_descriptor, write_ptr, characters_to_write);
	if(amount != characters_to_write)
	{
		perror("buffer_write: wrote less than expected");
	}
	buffer->count    = 0;
	buffer->data_ptr = buffer->data_start;

	return amount;
}

int buffer_ends_with_string(buffer_p buffer, char *string)
{
	if(string == NULL)
	{
		return false;
	}
	size_t size = strlen(string);
	if(size == 0)
	{ return true; }
	if(size > buffer->count)
	{ return false; }

	char *ptr = buffer->data_start + (buffer->count - size);

	while(*string != 0)
	{
		if(*string != *ptr)
		{
			return false;
		}
		ptr++;
		string++;
	}
	return true;
}

int buffer_indicates_start_of_capa(buffer_p buffer)
{
	char   *pointer = buffer->data_start;
	size_t count    = buffer->count;
	int    state    = 0;

	while(count > 0)
	{
		int c = *pointer;
		switch(state)
		{
			case 0: //read first letter
			{
				if(isupper(c))
				{
					state = 1;
				}
				else
				{
					return false;
				}
			}
				break;
			case 1: //read optional letters
			{
				if(!isupper(c))
				{
					if(c == '\r')
					{
						state = 2;
					}
					else
					{
						return false;
					}
				}
			}
				break;
			case 2: //read end
			{
				if(c == '\n')
				{
					return true;
				}
				else
				{
					return false;
				}
			}
				break;
		}
		pointer++;
		count--;
	}
	return false;
}

int buffer_indicates_start_of_list(buffer_p buffer)
{
	char   *pointer = buffer->data_start;
	size_t count    = buffer->count;
	int    state    = 0;

	while(count > 0)
	{
		int c = *pointer;
		switch(state)
		{
			case 0: //read int
			{
				if(isdigit(c))
				{
					state = 1;
				}
				else
				{
					return false;
				}
			}
				break;
			case 1: //read optional ints
			{
				if(!isdigit(c))
				{
					if(c == ' ')
					{
						state = 2;
					}
					else
					{
						return false;
					}
				}
			}
				break;
			case 2: //read until end
			{
				if(c == '\n')
				{ return true; }
				if(c == ' ')
				{ return false; }
			}
				break;
		}
		pointer++;
		count--;
	}
	return false;
}

int buffer_indicates_start_of_multiline_message(buffer_p buffer)
{
	return buffer_indicates_start_of_list(buffer) || buffer_indicates_start_of_capa(buffer);
}

int buffer_indicates_end_of_multiline_message(buffer_p buffer)
{
	return buffer->count == 3 && buffer_starts_with_string(".\r\n", buffer);
}

void buffer_remove_trailing_spaces(buffer_p buffer)
{
	char   *ptr       = buffer->data_start;
	size_t count      = buffer->count;
	int    last_space = -1;
	size_t i          = 0;
	while(i < count)
	{
		if(*(ptr + i) != '\n')
		{
			if(*(ptr + i) == ' ' || *(ptr + i) == '\r')
			{
				last_space = (last_space == -1) ? i : last_space;
			}
			else
			{
				last_space = -1;
			}
		}
		i++;
	}
	if(last_space > -1 && last_space + 1 < buffer->size)
	{
		*(ptr + last_space)     = '\n';
		*(ptr + last_space + 1) = 0;
		buffer->count = last_space + 1;
	}
}

int buffer_indicates_end_of_single_line_message(buffer_p buffer)
{
	return buffer_ends_with_string(buffer, "\r\n");
}

int buffer_read_until_string(int file_descriptor, buffer_p buffer, char *str) //\r\n\0 || \r\n\r\n\0
{
	int ret = buffer_fill_until_string(buffer,str);
	if(!ret)
	{
		buffer_big_read(file_descriptor, buffer);
		ret = buffer_fill_until_string(buffer,str);
	}
	return ret;
}

int buffer_is_line_buffered(buffer_p buffer)
{
	if(buffer->count < 4)
	{
		return false;
	}
	return *(buffer->data_start) == '.' && *(buffer->data_start + 1) != '\r';

}


int buffer_must_be_line_buffered(buffer_p buffer)
{
	return *(buffer->data_start) == '.';
}

int buffer_read_until_char_block(int file_descriptor, buffer_p buffer, char ch)
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

int buffer_read_until_char(int file_descriptor, buffer_p buffer, char ch)
{
	char str[2];
	str[0]=ch;
	str[1]=0;
	int ret = buffer_fill_until_string(buffer,str);
	if(!ret)
	{
		buffer_big_read(file_descriptor, buffer);
		ret = buffer_fill_until_string(buffer,"\n");
	}
	return ret;
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
				if(buffer - init_buffer >= size - len)
				{
					return -1;
				}
				sc = *buffer++;
			} while(sc != c);
		} while(strncmp(buffer, substring, len) != 0);
	}
	return buffer - init_buffer + len;
}

void print_big_buffer(buffer_p buff)
{
	int i;

	printf("--------------------------------------------------------\n");
	for(i = 0; i < buff->big_buffer->read_size; i++)
	{
		putchar(buff->big_buffer->data_start[i]);
	}
	printf("\n--------------------------------------------------------\n");

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
	int  characters_to_write = buffer->count;
	char *write_ptr          = buffer->data_start;
	int  i                   = 0;

	while(characters_to_write > 0)
	{
		*(string + i++) = *(write_ptr);
		write_ptr++;
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

int buffer_starts_with_string(char *string, buffer_p buffer)
{
	char *pointer = buffer->data_start;
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

void buffer_clean(buffer_p buff)
{
	buff->count    = 0;
	buff->data_ptr = buff->data_start;
}