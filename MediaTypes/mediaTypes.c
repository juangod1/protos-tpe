#include "include/mediaTypes.h"
#include "include/mtbuffer.h"
#include "stdbool.h"
#include "../Shared/include/lib.h"
#include "../Proxy/include/state.h"


int main()
{
    char* replacementMessage = getenv("FILTER_MSG");
    char* mediaList = getenv("FILTER_MEDIAS");
    mediaRangeEvaluator(replacementMessage, mediaList);

}

int mediaRangeEvaluator(char* replacementMessage, char* mediaList)
{
  char ** mediaRangeCompleteList = divideMediaRangeList(mediaList);

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
  size_t size= strlen(replacementMessage);
  media_type_state_machine(mediaRangeCompleteList, replacementMessage, size);
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

int media_type_state_machine(char ** media_type_complete_list, char * replacement_message, size_t replacement_message_size)
{
  buffer_p buffer;
  buffer_initialize(&buffer, BUFFER_SIZE);

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
        ;int amount = buffer_read_until_char_block(STDIN_FILENO, buffer, '\n');

        if(amount==0)
        {
            state=FINISHED;
            break;
        }
        if(new_line)
        {
          new_line=false;
          if((mime = mt_buffer_get_mime(buffer))!=NULL)
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
        boundaries[boundary_size]= mt_buffer_get_boundary(buffer);
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
        ;amount = buffer_read_until_char_block(STDIN_FILENO, buffer,'\n');
        if(amount==0)
        {
            state=FINISHED;
        }
        int flag=0;
        for(int i=0; i<boundary_size && !flag; i++)
        {
          char * boundary = boundaries[i];
          if(boundary!=NULL && mt_buffer_starts_with_boundary(boundary,buffer))
          {
            state=WRITE_LINE;
            new_line=true;
            flag=1;
          }
        }
        if(!flag)
        {
          mt_buffer_discard(buffer);
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


