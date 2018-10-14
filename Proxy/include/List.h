#ifndef LIST_H
#define LIST_H

typedef struct {
	state * state;
	node next;
} nodeStruct;

typedef struct {
	node head;
	size_t size;
} listStruct;

typedef listStruct* list;
typedef nodeStruct* node;
#include "include/List.h"
#include "include/stateMachine.h"

list new_list();
node new_node(state * s);
int put(list l, state * s);
state * get(list l, file_descriptor fd);
int remove(list l, state s);
void free_node(node n);
void free_list(list l);

#endif
