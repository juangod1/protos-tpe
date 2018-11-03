#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_t *buffer_p;

struct buffer_t
{
	size_t size;
	size_t count;
	char   *data_start;
	char   *data_ptr;
};

int buffer_initialize(buffer_p *buffer, size_t size);

int buffer_read_until_string(int file_descriptor, buffer_p buffer, char * str);

int buffer_finalize(buffer_p buffer);

int buffer_starts_with_string(char *string, buffer_p buffer);

int buffer_ends_with_string(buffer_p buffer, char * string);

int buffer_indicates_parsable_message(buffer_p buffer);

int buffer_write_string(char *string, buffer_p buffer);

int buffer_read_string(char *string, buffer_p buffer);

int buffer_indicates_end_of_multiline_message(buffer_p buffer);
int buffer_indicates_end_of_single_line_message(buffer_p buffer);
int buffer_read_string_endline(char* string, buffer_p buffer, int type);

int buffer_is_empty(buffer_p buffer);

int buffer_read(int file_descriptor, buffer_p buffer);

int buffer_write(int file_descriptor, buffer_p buffer);

int buffer_write_until_substring(int file_descriptor, buffer_p buffer, char *substring);

void print_buffer(buffer_p b);

int find_substring(char *buffer, int size, char *substring);

int buffer_read_until_char(int file_descriptor, buffer_p buffer, char ch);

int buffer_indicates_start_of_capa(buffer_p buffer);
int buffer_indicates_start_of_list(buffer_p buffer);
int buffer_indicates_start_of_multiline_message(buffer_p buffer);

#endif