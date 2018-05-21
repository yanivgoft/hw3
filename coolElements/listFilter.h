#ifndef DRY_LISTFILTER_LIBRARY_H
#define DRY_LISTFILTER_LIBRARY_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


typedef bool (*CmpFunction) (int a,int b);
typedef struct node_t* Node;
struct node_t {
    int n;
    Node next;
};
int findDivider(int index);
bool intCondition(int divider, Node cur_node);
Node findLastNode(Node node);
Node coolElements(Node* node_array,int n);

Node listFilter(Node cur_node,CmpFunction boolCond,int divider);
Node nodeCreate(int n);


#endif