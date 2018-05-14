#include "listFilter.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct node_t {
    int n;
    Node next;
} node_t*;
typedef bool (*CmpFunction) (int a,int b);
typedef struct node_t* Node;

Node listFilter(Node cur_node,CmpFunction boolCond,int divider);
void nodeCopy(Node source_node,dest_node);



Node listFilter(Node cur_node,CmpFunction boolCond,int divider) {
    if (node == NULL || boolCond == NULL) {
        return NULL;
    }

    while (!boolcond(cur_node->n,divider)) { //find first "good" node
        cur_node = cur_node->next;
        if(cur_node==NULL){
            return NULL;
        }
    }
    Node starting_node = malloc(sizeof((Node)));
    if (starting_node == NULL) {
        return NULL;;
    }
    nodeCopy(cur_node,starting_node);
    cur_node=cur_node->next;
    starting_node->next=cur_node;
    Node updated_node=starting_node;
    while (!(cur_node->next)) {
        if (!(boolCond(cur_node->n,divider))) {
            cur_node=cur_node->next;

        }
        else {
            Node temp_node=malloc(sizeof(Node));
            if(temp_node==NULL) {
                destroyList(starting_node);
            }
            nodeCopy(cur_node,temp);
            updated_node=temp;
            cur_node->next=cur_node;

        }

    }
    return starting_node;

}

void nodeCopy(Node source_node,Node dest_node){
    dest_node->n=source_node->n;
    dest_node->next=source_node->next;
}