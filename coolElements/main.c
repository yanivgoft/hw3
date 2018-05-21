#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "listFilter.h"
void printnodes(Node list);

int main() {
    Node list = malloc (sizeof(Node));
    Node list2 = malloc (sizeof(Node));
    Node list3 = malloc (sizeof(Node));
    Node list4 = malloc (sizeof(Node));
    Node list5 = malloc (sizeof(Node));
    Node list6;

    list->n = 1;
    list2->n = 2;
    list3->n = 3;
    list4->n = 4;



    list->next = list2;
    list2->next = list3;
    list3->next = NULL;

    list5->next = NULL;

    printnodes(list);
    printf ("\n\n");

    list5 = listFilter(list, intCondition);
    list4->next = list5;
    printnodes(list4);
    printf ("\n\n");
    printnodes(list);
    printf ("\n\n");

    Node a[2];
    a[0] = list;
    a[1] = list4;

    list6 = coolElements(a, 2);




    printnodes(list6);

    free(list);
    free(list2);
    free (list3);
    free (list4);
    free (list5);
    free (list6);


    return 0;
}

void printnodes(Node list){
    while(!list->next){
        printf("%d \n",list->n);
        list=list->next;
    }
}