#include "include/List.h"
#include "include/stateMachine.h"

list new_list(int (*comparator)(node a, node b)){
    list new = malloc(sizeof(listStruct));
    new->head = NULL;
    new->size = 0;
    new->comparator = comparator;
}

node new_node(state * s){
    node n = malloc(sizeof(nodeStruct));
    n->next=NULL;
    n->state=s;
    return n;
}

int put(list l, state * s){
    if(l!=NULL) {
        l->size++;
        return putRec(l, s);
    }
    return NULL;
}

node putRec(node curr, state * s){
    if(curr->next == NULL)
        return curr->next=new_node(s);
    return putRec(curr->next,s);
}

state get(list l, file_descriptor fd){
    getRec(l->head, fd);
}

state getRec(node curr, file_descriptor fd){
    if(curr==NULL)
        return NULL;

    if(curr->state->wait_read_fd == fd || curr->state->wait_write_fd == fd){
        return curr;
    }

    return getRec(curr->next,fd);
}

int remove(list l, state s){
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

    if(curr->next->state == s){
        node n = curr->next->next;
        free_node(curr->next);
        curr->next=n;
        return 0;
    }
}

void free_node(node n){
    free_state(n->state);
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