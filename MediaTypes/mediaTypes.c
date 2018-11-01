#include "mediaTypes.h"
#include "buffer.h"
#include "stdbool.h"


char * types[TYPE_ARRAY_LENGTH]= {"application", "audio", "example", "font", "image", "message", "model", "multipart", "text", "video"};


int main(int argc, char ** args)
{
  if(argc!=2)
  {
    printf("Program must be executed as follows ./prog '[ListOfMediaRanges]'\n");
    exit(1);
  }
  if(strcmp(args[1],"-t")==0) //testmode
  {
    mainTester();
  }
  else
  {
    mediaRangeEvaluator(argc, args);
  }
}


int mediaRangeEvaluator(int argc, char ** args)
{
  char ** mediaRangeCompleteList = divideMediaRangeList(args[1]);
  char * mediaRangeComplete;
  int i=0;
  while((mediaRangeComplete=mediaRangeCompleteList[i])!=NULL)
  {
    char ** splitMediaRange = divideMediaType(mediaRangeComplete);
    if(!isValidMediaType(splitMediaRange))
    {
      printf("'%s' is not a valid mediaRange. Exiting program...\n",mediaRangeComplete);
      recursiveDoublePointerFree(splitMediaRange);
      exit(1);
    }
    i++;
    recursiveDoublePointerFree(splitMediaRange);
  }
  media_type_state_machine(mediaRangeCompleteList, "test", 5);
  recursiveDoublePointerFree(mediaRangeCompleteList);
}

int evaluate_mime(char * mime, char ** media_type_complete_list)
{
    int ret=0;
    char ** splitMediaType = divideMediaType(mime);
    if(!isValidMediaType(splitMediaType))
    {
      ret = OK;
    }
    int j=0;
    char * mediaRangeComplete;
    while(!ret && (mediaRangeComplete=media_type_complete_list[j]))
    {
      char ** splitMediaRange = divideMediaType(mediaRangeComplete);
      if(mediaTypeBelongsToMediaRange(splitMediaType,splitMediaRange))
      {
        ret = PROHIBITED;
      }
      j++;
      recursiveDoublePointerFree(splitMediaRange);
    }
    char * multipart_str = calloc(1,10);
    memcpy(multipart_str,"multipart",10);
    char * splitMultipart[]={multipart_str,NULL};
    if(!ret && mediaTypeBelongsToMediaRange(splitMediaType,splitMultipart))
    {
      ret = MULTIPART;
    }
    free(multipart_str);
    recursiveDoublePointerFree(splitMediaType);

    return (ret)?ret:OK;
}

int media_type_state_machine(char ** media_type_complete_list, char * replacement_message, int replacement_message_size)
{
  buffer_p buffer;
  buffer_initialize(&buffer, buffer_size);

  int finished=false;
  int state=READ_LINE;
  int new_line = true;

  char * mime = NULL;
  char * boundary = NULL;
  int boundary_size=0;
  char ** boundaries = malloc(sizeof(char *)*BOUNDARIES);

  while(!finished)
  {
    switch(state)
    {
      case READ_LINE:
        ;int amount = buffer_read_until_char(STDIN_FILENO, buffer, '\n');
        if(amount==0)
        {
            state=FINISHED;
            break;
        }
        if(new_line)
        {
          new_line=false;
          if((mime = buffer_get_mime(buffer))!=NULL)
          {
            state = EVAL_MIME;
          }
          else
          {
            state=WRITE_LINE;
          }
        }
        else
        {
          state=WRITE_LINE;
        }

      break;
      case WRITE_LINE:
        buffer_write(STDOUT_FILENO,buffer);
        state=READ_LINE;
        new_line=true;
      break;
      case EVAL_MIME:
        ;int evaluation = evaluate_mime(mime, media_type_complete_list);
        if(evaluation==OK)
        {
          state=WRITE_LINE;
        }
        if(evaluation==PROHIBITED)
        {
          state=WRITE_LINE_BEFORE_REPLACEMENT;
        }
        if(evaluation==MULTIPART)
        {
          state=BOUNDARY_SEARCH;
        }
        free(mime);
      break;
      case BOUNDARY_SEARCH:
        boundaries[boundary_size]=buffer_get_boundary(buffer);
        if(boundaries[boundary_size]!=NULL)
        {
          boundary_size++;
          if(boundary_size%BOUNDARIES==0)
          {
            boundaries=realloc(boundaries,boundary_size+BOUNDARIES);
          }
        }
        state=WRITE_LINE;
      break;
      case WRITE_LINE_BEFORE_REPLACEMENT:
        buffer_write(STDOUT_FILENO,buffer);
        state=WRITE_REPLACEMENT;
      break;
      case WRITE_REPLACEMENT:
        write(STDOUT_FILENO, replacement_message, replacement_message_size);
        write(STDOUT_FILENO,"\n", 1);
        state=READ_LINE_AFTER_REPLACEMENT;
      break;
      case READ_LINE_AFTER_REPLACEMENT:
        ;amount = buffer_read_until_char(STDIN_FILENO, buffer,'\n');
        if(amount==0)
        {
            state=FINISHED;
        }
        int flag=0;
        for(int i=0; i<boundary_size && !flag; i++)
        {
          char * boundary = boundaries[i];
          if(boundary!=NULL && buffer_starts_with_boundary(boundary,buffer))
          {
            state=WRITE_LINE;
            new_line=true;
            flag=1;
          }
        }
        if(!flag)
        {
          buffer_discard(buffer);
        }

      break;
      case FINISHED:
        finished=true;
      break;
      default:
        printf("Invalid State, error\n");
        exit(1);
      break;
    }
  }
  for(int i=0; i<boundary_size; i++)
  {
    free(boundaries[i]);
  }
  free(boundaries);
  buffer_finalize(buffer);

}

int fetchInputFromStdin(char ** bufferPosition)
{
  char c;
  int counter=0;
  char * buffer = malloc(INITIAL_INPUT_SIZE);
  int size=INITIAL_INPUT_SIZE;
  while((c=getchar())!=EOF){
    if(counter>=size){
      size+=INITIAL_INPUT_SIZE;
      buffer = realloc(buffer, size);
    }
    *(buffer+counter)=c;
    counter++;
  }
  if(counter>=size){
    size+=1;
    buffer = realloc(buffer, size);
  }
  *(buffer+counter)=0;
  *bufferPosition=buffer;
  return counter;
}

int toNLowerString(char * lowerCaseCopy, char * original, int n)
{
  char c = *original;
  int i=0;
  while(c!=0 && i<n)
  {
    *(lowerCaseCopy+i)=tolower(c);
    c=*(original+ ++i);
  }
  *(lowerCaseCopy+i)=0;
  if(i>=n)
  {
    return 1;
  }
  return 0;
}

int isType(char * type)
{
  char * lowerCaseCopy = malloc(MAX_TYPE_SIZE);
  if(toNLowerString(lowerCaseCopy, type, MAX_TYPE_SIZE)){
    free(lowerCaseCopy);
    return 0;
  }
  for(int i=0; i<TYPE_ARRAY_LENGTH;i++)
  {
      if(!strcmp(lowerCaseCopy,types[i]))
      {
        free(lowerCaseCopy);
        return 1;
      }
  }
  free(lowerCaseCopy);
  return 0;
}

char ** divideStrByDelimeter(char * string, char * delimeter)
{
  char * nextToken;
  char ** dictionary= calloc(INITIAL_DICTIONARY_SIZE, sizeof(char *));

  if(string==NULL)
  {
    return dictionary;
  }

  char * copy=my_strdup(string);
  char * tofree=copy;

  int i=0, size=INITIAL_DICTIONARY_SIZE;

  while((nextToken = my_strsep(&copy, *delimeter)))
  {
    if(strcmp(nextToken,"")==0) break;
    if(i>=size)
    {
      size+=INITIAL_DICTIONARY_SIZE;
      dictionary=realloc(dictionary,size *sizeof(char *));
    }
    dictionary[i]=my_strdup(nextToken);
    i++;
  }

  free(tofree);

  if(i>=size)
  {
    size+=1;
    dictionary=realloc(dictionary,size * sizeof(char *));
  }
  dictionary[i]=NULL;
  return dictionary;
}

char ** divideMediaType(char * mediaType)
{
  return divideStrByDelimeter(mediaType, "/");
}

char ** divideUserInputByLine(char * userInput)
{
  return divideStrByDelimeter(userInput,"\n");
}

char ** divideMediaRangeList(char * mediaTypeList)
{
  return divideStrByDelimeter(mediaTypeList,",");
}

int mediaTypeBelongsToMediaRange(char ** mediaType, char ** mediaRange)
{
  if(mediaType==NULL || mediaRange==NULL)
  {
    return 0;
  }

  char * mediaTypeToken;
  char * mediaRangeToken;

  int i=0;
  while(1)
  {
    mediaTypeToken = mediaType[i];
    mediaRangeToken = mediaRange[i];
    if(mediaRangeToken==NULL)
    {
      return 1;
    }
    if(mediaTypeToken==NULL)
    {
      return 0;
    }
    if(strcmp(mediaRangeToken,"*")==0)
    {
      return 1;
    }
    if(strcmp(mediaRangeToken,mediaTypeToken)!=0)
    {
      return 0;
    }
    i++;
  }

}

void recursiveDoublePointerFree(char ** splitMediaType)
{
  if(splitMediaType==NULL){
    return;
  }
  int i=0;
  while((splitMediaType[i])!=NULL)
  {
    free(splitMediaType[i]);
    splitMediaType[i]=NULL;
    i++;
  }
  free(splitMediaType);
  splitMediaType=NULL;
}

int isValidMediaType(char ** mediaType)
{
  char ** dictionary=mediaType;
  if(dictionary[0]==NULL ||!isType(dictionary[0]))
  {
    return 0;
  }
  return 1;
}

//https://stackoverflow.com/questions/32944390/what-is-the-rationale-for-not-including-strdup-in-the-c-standard
char *my_strdup(const char *s)
{
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}


char *my_strsep(char ** string_ptr, char delimeter )
{
  if(string_ptr==NULL || *string_ptr==NULL)
  {
    return NULL;
  }
  char * ptr = *string_ptr;
  char * ret = *string_ptr;
  while(*ptr!=0 && *ptr!=delimeter)
  {
    *ptr++;
  }
  if(*ptr==delimeter)
  {
    *ptr=0;
    *string_ptr=ptr+1;
    return ret;
  }
  *string_ptr=NULL;
  return ret;
}
