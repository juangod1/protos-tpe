#include "include/list.h"
#include "include/stateMachine.h"
#include <stdlib.h>
#include <sys/select.h>

list new_list()
{
	list new = malloc(sizeof(struct listStruct));
	new->head = NULL;
	new->size = 0;
	return new;
}

node new_node(state s)
{
	node n = malloc(sizeof(struct nodeStruct));
	n->next = NULL;
	n->st   = s;
	return n;
}

void put(list l, state s)
{
	if(l != NULL)
	{
		l->size++;
		if(l->head == NULL)
		{
			l->head = new_node(s);
			return;
		}
		putRec(l->head, s);
	}
}

node putRec(node curr, state s)
{
	if(curr->next == NULL)
	{
		return curr->next = new_node(s);
	}
	return putRec(curr->next, s);
}

state getRec(node curr, file_descriptor fd, int is_read)
{
	if(curr == NULL)
	{
		return NULL;
	}

	int i;

	if(is_read)
	{
		for(i = 0; i < 3; i++)
		{
			if(curr->st->read_fds[i] == fd)
			{
				return curr->st;
			}
		}
	}
	else
	{
		for(i = 0; i < 3; i++)
		{
			if(curr->st->write_fds[i] == fd)
			{
				return curr->st;
			}
		}
	}

	return getRec(curr->next, fd, is_read);
}

state get(list l, file_descriptor fd, int is_read)
{
	return getRec(l->head, fd, is_read);
}

int removeRec(node curr, state s, list l)
{
	if(curr->st == s)
	{
		free_node(curr->next);
		l->head = NULL;
		return 0;
	}

	if(curr->next == NULL)
	{
		return -1;
	}

	if(curr->next->st == s)
	{
		node n = curr->next->next;
		free_node(curr->next);
		curr->next = n;
		return 0;
	}

	return removeRec(curr->next, s, l);
}

int remove_node(list l, state s)
{
	if(l == NULL)
	{
		return -1;
	}

	if(removeRec(l->head, s, l))
	{
		return -1;
	}
	else
	{
		l->size--;
		return 0;
	}
}

void free_node(node n)
{
	free_state(n->st);
	free(n);
}

void free_list(list l)
{
	if(l == NULL)
	{
		return;
	}
	free_list_rec(l->head);
}

void free_list_rec(node curr)
{
	if(curr == NULL)
	{
		return;
	}

	free_list_rec(curr->next);

	free_node(curr);
}
