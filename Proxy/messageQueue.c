#include <stdio.h>
#include <strings.h>
#include "include/messageQueue.h"

void message_queue_qdd(int has_transform, int is_multiline,queueEntry * q)
{
    if((*q)==NULL)
    {
        (*q)=malloc(sizeof(struct queue_entry_struct));
        (*q)->data=malloc(sizeof(struct message_data_struct));
        (*q)->data->has_transform=has_transform;
        (*q)->data->is_multiline=is_multiline;
        (*q)->next=NULL;
    }
    else
    {
        processQueueAdd(pid_P, &((*q)->next));
    }
}

message_data message_queue_poll(queueEntry * q)
{
    if(q==NULL || (*q)==NULL){
        return NULL;
    }
    queueEntry aux = (*q)->next;
    message_data ret =(*q)->data;

    (*q)->data=NULL;
    (*q)->next=NULL;
    free((*q));

    (*q)=aux;
    return ret;
}

void message_queue_finalize(queueEntry * q)
{
    while(processQueueRemove(q)!=NULL);
}
