#include <stdio.h>
#include "map_mtm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
typedef struct mapNode *Node;
struct mapNode{
    MapDataElement data;
    MapKeyElement key;
    Node next;
    Node prev;
};


struct Map_t{
    Node iterator;
    Node first;
    copyMapDataElements copy_data;
    copyMapKeyElements copy_key;
    compareMapKeyElements compare_key;
    freeMapDataElements free_data;
    freeMapKeyElements free_key;
};

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements){
    if(copyDataElement==NULL || copyKeyElement==NULL || freeDataElement==NULL
       || freeKeyElement==NULL || compareKeyElements==NULL){
        return NULL;
    }
    Map map=malloc(sizeof( *map));
    if(!map){
        return NULL;
    }
    map->copy_data=copyDataElement;
    map->copy_key=copyKeyElement;
    map->free_data=freeDataElement;
    map->free_key=freeKeyElement;
    map->compare_key=compareKeyElements;
    map->iterator=NULL;
    map->first=NULL;
    return map;
}

void static nodeDestroy(Map m, Node n){
    m->free_data(n->data);
    m->free_key(n->key);
    free(n);
    n=NULL;
}

void mapDestroy(Map map){
    if(map==NULL){
        return;
    }
    map->iterator=map->first;
    while(map->iterator!=NULL){
        Node temp=map->iterator->next;
        nodeDestroy(map, map->iterator);
        map->iterator=temp;
    }
    free(map);
    map=NULL;

}


Map mapCopy(Map map){
    if(!map){
        return NULL;
    }
    Map copy=mapCreate(map->copy_data, map->copy_key, map->free_data,
                       map->free_key, map->compare_key);
    if(!copy){
        return NULL;
    }
    Node temp=map->first;
    while(temp){
        MapKeyElement key=temp->key;
        MapDataElement data=temp->data;
        mapPut(copy,key,data);
        temp=temp->next;
    }

    return copy;
}

int mapGetSize(Map map){
    if(!map){
        return -1;
    }
    int counter=1;
    Node temp=map->iterator;
    if(map->first==NULL && map->first==NULL){
        return 0;
    }
    while(temp){
        counter++;
        temp=temp->prev;
    }
    while(map->iterator){
        counter++;
        map->iterator=map->iterator->next;
    }
    return counter;
}
static Node findNodeLocation(Map map, MapKeyElement element){
    assert(map && element && map->first);
    Node temp=map->first;
    if(!map->first){
        return NULL;
    }
    if(map->compare_key(element, temp->key)>0) {
        while (temp->next && map->compare_key(element, temp->key) > 0) {
            temp = temp->next;
        }
    }
    else {
        while (temp->prev && (map->compare_key(element, temp->key) < 0)) {
            temp = temp->prev;
        }
    }
    return temp;
}

bool mapContains(Map map, MapKeyElement element) {
    if (!map || !element) {
        return false;
    }
    if(mapGetSize(map)==0){
        return false;
    }
    if(!findNodeLocation(map,element)){
        return false;
    }
    if (map->compare_key(findNodeLocation(map, element)->key, element)) {
        return false;
    }
    return true;
}

static Node nodeCreate(Map map,MapDataElement dataElement,
       MapKeyElement keyElement,MapResult* result){
    Node new_node=malloc(sizeof(*new_node));
    if(!new_node){
        *result= MAP_OUT_OF_MEMORY;
        return NULL;
    }
    new_node->data=map->copy_data(dataElement);
    if(!new_node->data){
        free(new_node);
        new_node=NULL;
        *result= MAP_OUT_OF_MEMORY;
        return NULL;

    }
    new_node->key=map->copy_key(keyElement);
    if(!new_node->key){
        map->free_data(new_node->data);
        free(new_node);
        new_node=NULL;
        *result= MAP_OUT_OF_MEMORY;
        return NULL;

    }
    *result= MAP_SUCCESS;
    return new_node;
}

MapResult mapPut(Map map, MapKeyElement keyElement,
                 MapDataElement dataElement){
    if(!map || !keyElement || !dataElement){
        return MAP_NULL_ARGUMENT;
    }
    Node new_node;
    MapResult result;
    if(map->first==NULL){ //first node
        new_node=nodeCreate(map,dataElement,keyElement,&result);
        if(result!=MAP_SUCCESS){
            return result;
        }
        map->first=new_node;
        map->first->next=NULL;
        map->first->prev=NULL;
        map->iterator=NULL;
        return result;
    }
    if(!map->iterator){
        map->iterator=map->first;
    }
    Node temp=findNodeLocation(map,keyElement);
    int compare=map->compare_key(temp->key,keyElement);

    if(compare==0){
        map->free_data(temp->data);
        temp->data=map->copy_data(dataElement);
    }
    else {
        new_node=nodeCreate(map,dataElement,keyElement,&result);
        if(result!=MAP_SUCCESS){
            map->iterator=NULL;
            return result;
        }
        if(compare>0) {
            if(temp->prev) {
                temp->prev->next = new_node;
                new_node->prev = temp->prev;

            }
            else {
                new_node->prev=NULL;
                map->first=new_node;
            }
            new_node->next = temp;
            temp->prev = new_node;
        }
        else {
            if (temp->next) {
                temp->next->prev = new_node;
                new_node->next = temp->next;
            }
            else {
                new_node->next = NULL;
            }
            temp->next = new_node;
            new_node->prev = temp;
        }
    }
    map->iterator=NULL;
    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement) {
    if(!map || !keyElement){
        return NULL;
    }
    Node temp = findNodeLocation(map, keyElement);
    return temp->data;
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
    if(!map || !keyElement){
        return MAP_NULL_ARGUMENT;
    }
    Node temp=findNodeLocation(map,keyElement);
    if(map->compare_key(keyElement, temp->key)){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    if(temp->prev !=NULL && temp->next!=NULL){
        temp->prev->next=temp->next;
        temp->next->prev=temp->prev;
    }
    if(temp->prev==NULL && temp->next!=NULL){
    //    map->first=temp->next;
        temp->next->prev=NULL;
    }
    if(temp->prev!=NULL && temp->next==NULL){
        temp->prev->next=NULL;
    }
    map->free_data(temp->data);
    map->free_key(temp->key);
    free(temp);
    temp=NULL;
    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map){
    if(!map){
        return NULL;
    }
    if(mapGetSize(map)==0){
        return NULL;
    }
    map->iterator=map->first;
    return map->first->key;
}


MapKeyElement mapGetNext(Map map){
    if(!map){
        return NULL;
    }
    if(!map->iterator){
        return NULL;
    }
    if(!map->iterator->next){
        return NULL;
    }
    map->iterator=map->iterator->next;
    if(!map->iterator){
        return NULL;
    }
    return map->iterator->key;
}


MapResult mapClear(Map map){
    if(!map){
        return MAP_NULL_ARGUMENT;
    }
    map->iterator=map->first;
    while(map->iterator->next!=NULL){
        map->iterator=map->iterator->next;
        mapRemove(map, map->iterator->prev->key);
    }
    MapResult result=mapRemove(map,map->iterator->key);
    return result;
}