#include "listFilter.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


Node listFilter(Node list,CmpFunction boolCond,int divider) {
    if (list == NULL || boolCond == NULL) {
        return NULL;
    }

    while (!boolCond(divider,list->n)) { //find first "good" node
        list = list->next;
    }
    if(!list->next && !boolCond(divider,list->n){//only one "good" node
        return NULL;
    }
    if(!list->next){
        return list;
    }
    Node starting_node = nodeCreate(list->n);
    if (starting_node == NULL) {
        return NULL;;
    }
    list=list->next;
    starting_node->next=list;
    Node updated_node=starting_node;
    while (!(list->next)) {
        if (!(boolCond(divider,list->n))) {
            list=list->next;

        }
        else {
            Node temp_node=nodeCreate(list->n));
            if(temp_node==NULL) {
               // destroyList(starting_node);
                return NULL;
            }
            updated_node->next=temp_node;
            list=list->next;

        }

    }
    return starting_node;

}

Node nodeCreate(int n){
    Node new_node=malloc(sizeof(new_node));
    if(!new_node){
        return NULL;
    }
    new_node->n=n;
    new_node->next=NULL;
    return new_node;
}