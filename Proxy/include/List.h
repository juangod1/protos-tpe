#ifndef LIST_H
#define LIST_H

typedef struct {
	state * state;
	node next;
} nodeStruct;

typedef struct {
	int (*comparator)(node a, node b);
	node head;
	size_t size;
} listStruct;

typedef listStruct* list;
typedef nodeStruct* node;

#endif
