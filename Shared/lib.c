#include <ctype.h>
#include <string.h>
#include "include/lib.h"


char *types[TYPE_ARRAY_LENGTH] = {"application", "audio", "example", "font", "image", "message", "model", "multipart",
                                  "text", "video"};

//https://stackoverflow.com/questions/32944390/what-is-the-rationale-for-not-including-strdup-in-the-c-standard
char *my_strdup(const char *s)
{
	size_t size = strlen(s) + 1;
	char   *p   = malloc(size);
	if(p)
	{
		memcpy(p, s, size);
	}
	return p;
}


char *my_strsep(char **string_ptr, char delimeter)
{
	if(string_ptr == NULL || *string_ptr == NULL)
	{
		return NULL;
	}
	char *ptr   = *string_ptr;
	char *ret   = *string_ptr;

	while(*ptr != 0 && *ptr != delimeter)
	{ ptr++; }

	if(*ptr == delimeter)
	{
		*ptr        = 0;
		*string_ptr = ptr + 1;
		return ret;
	}
	*string_ptr = NULL;
	return ret;
}


size_t fetchInputFromStdin(char **bufferPosition, size_t size)
{
	return fetchInputFromFile(bufferPosition, stdin, size);
}

size_t fetchInputFromFile(char **bufferPosition, FILE *f, size_t size)
{
	int    c;
	size_t counter = 0;
	while((c = fgetc(f)) != EOF)
	{
		if(counter >= size)
		{
			size_t init = size;
			size += INITIAL_INPUT_SIZE;
			(*bufferPosition) = realloc((*bufferPosition), size);
			while(init < size)
			{ (*bufferPosition)[init++] = 0; }
		}
		*((*bufferPosition) + counter) = (char) c;
		counter++;
	}
	return counter;
}

size_t fetchLineFromStdin(char **bufferPosition, size_t size)
{
	return fetchLineFromFile(bufferPosition, stdin, size);
}


size_t fetchLineFromFile(char **bufferPosition, FILE *f, size_t size)
{
	int    c;
	size_t counter = 0;
	while((c = fgetc(f)) != '\n' && c != 0)
	{
		if(counter >= size)
		{
			size_t init = size;
			size += INITIAL_INPUT_SIZE;
			(*bufferPosition) = realloc((*bufferPosition), size);
			while(init < size)
			{ (*bufferPosition)[init++] = 0; }
		}
		*((*bufferPosition) + counter) = (char) c;
		counter++;
	}
	return counter;
}


int toNLowerString(char *lowerCaseCopy, char *original, int n)
{
	char c = *original;
	int  i = 0;
	while(c != 0 && i < n)
	{
		*(lowerCaseCopy + i) = tolower(c);
		c = *(original + ++i);
	}
	*(lowerCaseCopy + i) = 0;
	if(i >= n)
	{
		return 1;
	}
	return 0;
}

int isType(char *type)
{
	char *lowerCaseCopy = malloc(MAX_TYPE_SIZE);
	if(toNLowerString(lowerCaseCopy, type, MAX_TYPE_SIZE))
	{
		free(lowerCaseCopy);
		return 0;
	}
	for(int i = 0; i < TYPE_ARRAY_LENGTH; i++)
	{
		if(!strcmp(lowerCaseCopy, types[i]))
		{
			free(lowerCaseCopy);
			return 1;
		}
	}
	free(lowerCaseCopy);
	return 0;
}

char **divideStrByDelimeter(char *string, char delimeter)
{
	char *nextToken;
	char **dictionary = calloc(INITIAL_DICTIONARY_SIZE, sizeof(char *));

	if(string == NULL)
	{
		return dictionary;
	}

	char *copy      = my_strdup(string);
	char *tofree    = copy;

	int i = 0, size = INITIAL_DICTIONARY_SIZE;

	while((nextToken = my_strsep(&copy, delimeter)))
	{
		if(strcmp(nextToken, "") == 0)
		{ break; }
		if(i >= size)
		{
			size += INITIAL_DICTIONARY_SIZE;
			dictionary = realloc(dictionary, size * sizeof(char *));
		}
		dictionary[i] = my_strdup(nextToken);
		i++;
	}

	free(tofree);

	if(i >= size)
	{
		size += 1;
		dictionary = realloc(dictionary, size * sizeof(char *));
	}
	dictionary[i] = NULL;
	return dictionary;
}

char **divideMediaType(char *mediaType)
{
	return divideStrByDelimeter(mediaType, '/');
}

char **divideUserInputByLine(char *userInput)
{
	return divideStrByDelimeter(userInput, '\n');
}

char **divideMediaRangeList(char *mediaTypeList)
{
	return divideStrByDelimeter(mediaTypeList, ',');
}

int mediaTypeBelongsToMediaRange(char **mediaType, char **mediaRange)
{

    if(mediaType == NULL || mediaRange == NULL)
	{
		return 0;
	}

	char *mediaTypeToken;
	char *mediaRangeToken;

	int i = 0;
	while(1)
	{
		mediaTypeToken  = mediaType[i];
		mediaRangeToken = mediaRange[i];

		if(mediaRangeToken == NULL)
		{
			return 1;
		}
		if(mediaTypeToken == NULL)
		{
			return 0;
		}
		if(strcmp(mediaRangeToken, "*") == 0)
		{
			return 1;
		}
		if(strcmp(mediaRangeToken, mediaTypeToken) != 0)
		{
			return 0;
		}
		i++;
	}

}

void recursiveDoublePointerFree(char **splitMediaType)
{
	if(splitMediaType == NULL)
	{
		return;
	}
	int i = 0;
	while((splitMediaType[i]) != NULL)
	{
		free(splitMediaType[i]);
		splitMediaType[i] = NULL;
		i++;
	}
	free(splitMediaType);
}

int isValidMediaType(char **mediaType)
{
	char **dictionary = mediaType;
	if(dictionary[0] == NULL || !isType(dictionary[0]))
	{
		return 0;
	}
	return 1;
}