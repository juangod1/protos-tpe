#ifndef PROTOS_TPE_MESSAGEQUEUE_H
#define PROTOS_TPE_MESSAGEQUEUE_H


typedef struct message_data_struct * message_data;
struct message_data_struct
{
    int has_transform;
    int is_multiline;
};

typedef struct queue_entry_struct * queue_entry;
struct queue_entry_struct
{
    message_data data;
    queue_entry next;
};

void message_queue_qdd(int has_transform, int is_multiline,queue_entry * q);
message_data message_queue_poll(queue_entry * q);
void message_queue_finalize(queue_entry * q);

#endif