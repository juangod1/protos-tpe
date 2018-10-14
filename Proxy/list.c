#include "include/list.h"
#include "include/stateMachine.h"
#include <stdlib.h>

list new_list(){
    list new = malloc(sizeof(struct listStruct));
    new->head = NULL;
    new->size = 0;
}

node new_node(state s){
    node n = malloc(sizeof(struct nodeStruct));
    n->next=NULL;
    n->st=s;
    return n;
}

node put(list l, state s)
{
    if(l!=NULL) {
        l->size++;
        return putRec(l->head, s);
    }
    return NULL;
}

node putRec(node curr, state s){
    if(curr->next == NULL)
        return curr->next=new_node(s);
    return putRec(curr->next,s);
}

state get(list l, file_descriptor fd){
    return getRec(l->head, fd);
}

state getRec(node curr, file_descriptor fd){
    if(curr==NULL)
        return NULL;

    if(curr->st->wait_read_fd == fd || curr->st->wait_write_fd == fd){
        return curr->st;
    }

    return getRec(curr->next,fd);
}

int remove_node(list l, state s){
    if(l==NULL)
        return -1;

    if(removeRec(l->head,s)){
        return -1;
    }
    else{
        l->size--;
        return 0;
    }
}

int removeRec(node curr, state s){
    if(curr->next==NULL)
        return -1;

    if(curr->next->st == s){
        node n = curr->next->next;
        free_node(curr->next);
        curr->next=n;
        return 0;
    }
}

void free_node(node n){
    free_state(n->st);
    free(n);
}

void free_list(list l){
    if(l==NULL)
        return;
    free_list_rec(l->head);
}

void free_list_rec(node curr){
    if(curr==NULL)
        return;

    free_list_rec(curr->next);

    free_node(curr);
}