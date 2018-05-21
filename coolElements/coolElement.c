#include "listFilter.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


int findDivider(int index);
bool intCondition(int divider, Node cur_node);
Node findLastNode(Node node);
Node coolElements(Node* node_array,int n);

Node coolElements(Node* node_array,int n) {
    int divider;
    Node list_begin=malloc(sizeof((list_begin)));
    if(list_begin==NULL){
        return NULL;
    }
    Node list_cur=list_begin;
    CmpFunction boolCond=intCondition;
    for(int i=0;i<n;i++){
        divider=findDivider(i);
        Node temp=listFilter(node_array[i],boolCond,divider);
        if(temp==NULL){
            continue;
        }
        list_cur->next=findLastNode(temp);//getting the lest node from the "good" list, shirshur
    }
    return list_begin;
}

int findDivider(int index){
    int result=index%3;
    return result+1;
}

bool intCondition(int divider, int cur_node) {
    if(cur_node%divider==0){
        return true;
    }
    return false;

}

Node findLastNode(Node node){//should we use malloc???
    while(node->next!=NULL){
        node=node->next;
    }
    return node;
}