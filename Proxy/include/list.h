#ifndef PROTOS_TPE_LIST_H
#define PROTOS_TPE_LIST_H

#include "unistd.h"
#include "state.h"

typedef struct nodeStruct *node;

struct nodeStruct
{
	state st;
	node  next;
};

struct listStruct
{
	node   head;
	size_t size;
};

typedef struct listStruct *list;

list new_list();

node new_node(state s);

void put(list l, state s);

node putRec(node curr, state s);

state get(list l, file_descriptor fd, int is_read);

int remove_node(list l, state s);

void free_node(node n);

void free_list(list l);

void free_list_rec(node curr);

#endif
