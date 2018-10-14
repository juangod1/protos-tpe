#ifndef PROTOS_TPE_LIST_H
#define PROTOS_TPE_LIST_H

#include "unistd.h"
#include "state.h"

typedef struct listStruct * list;
typedef struct nodeStruct * node;

struct nodeStruct{
	state st;
	node next;
};

struct listStruct{
	node head;
	size_t size;
};

list new_list();
node new_node(state s);
int put(list l, state s);
state get(list l, file_descriptor fd);
int remove_node(list l, state s);
void free_node(node n);
void free_list(list l);

#endif
