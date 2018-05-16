#include <stdio.h>
#include "map_mtm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct mapNode *Node;
struct mapNode{
    MapDataElement data;
    MapKeyElement key;
    Node next;
    Node prev;
};


struct Map_t{
    Node iterator;
    copyMapDataElements copy_data;
    copyMapKeyElements copy_key;
    compareMapKeyElements compare_key;
    freeMapDataElements free_data;
    freeMapKeyElements free_key;
};

Map mapCreate(copyMapDataElements copyDataElement, copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement, freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements){
    Map map=malloc(sizeof( *map));
    if(!map){
        return NULL;
    }
    if(copyDataElement==NULL || copyKeyElement==NULL || freeDataElement==NULL
            || freeKeyElement==NULL || compareKeyElements==NULL){
        return NULL;
    }
    map->copy_data=copyDataElement;
    map->copy_key=copyKeyElement;
    map->free_data=freeDataElement;
    map->free_key=freeKeyElement;
    map->compare_key=compareKeyElements;
    map->iterator=NULL;
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
    while(map->iterator!=NULL){
        Node temp=map->iterator->next;
        nodeDestroy(map, map->iterator);
        map->iterator=temp;
    }
    free(map);
    map=NULL;

}

Map mapCopy(Map map){
    Map copy=mapCreate(map->copy_data, map->copy_key, map->free_data,
                       map->free_key, map->compare_key);
    if(!copy){
        return NULL;
    }
    return copy;
}

int mapGetSize(Map map){
    if(!map){
        return -1;
    }
    int counter=1;
    Node temp=map->iterator;
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
    while(map->compare_key(element, map->iterator->key)>0) {
        map->iterator = map->iterator->next;
    }
    while(map->compare_key(element,map->iterator->key)<0){
        map->iterator=map->iterator->prev;
    }
    return map->iterator;

}

bool mapContains(Map map, MapKeyElement element) {
    if (!map || !element) {
        return false;
    }
    if (map->compare_key(findNodeLocation(map, element)->key, element)) {
        return false;
    }
    return true;
}

MapResult mapPut(Map map, MapKeyElement keyElement,
                 MapDataElement dataElement){
    if(!map){
        return MAP_NULL_ARGUMENT;
    }
    if(keyElement==NULL || dataElement==NULL){
        return MAP_NULL_ARGUMENT;
    }
    Node temp=findNodeLocation(map,keyElement);
    int comapre=map->compare_key(temp->key,keyElement);
    Node new_node;
    if(comapre==0){
        map->free_data(temp->data);
        temp->data=map->copy_data(dataElement);
    }
    else {
        new_node=malloc(sizeof(*new_node));
        if(!new_node){
            return MAP_OUT_OF_MEMORY;
        }
        new_node->data=map->copy_data(dataElement);
        if(!new_node->data){
            free(new_node);
            return  MAP_OUT_OF_MEMORY;
        }
        new_node->key=map->copy_key(keyElement);
        if(!new_node->key){
            map->free_data(new_node->data);
            free(new_node);
            return MAP_OUT_OF_MEMORY;
        }
        if(comapre>0) {
            if(temp->prev) {
                temp->prev->next = new_node;
                new_node->prev = temp->prev;
            }
            else {
                new_node->prev=NULL;
            }
            if(temp->next) {
                new_node->next = temp;
                temp->prev = new_node;
            }
            else {
                new_node->next=NULL;
            }
        }
        else {
            if (temp->next) {
                temp->next->prev = new_node;
                new_node->next = temp->next;
            } else {
                new_node->next = NULL;
            }
            if (temp->prev) {
                temp->next = new_node;
                new_node->prev = temp;
            }
            else{
                new_node->prev=NULL;
            }
        }
    }
    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement) {
    if(!map || !keyElement){
        return NULL;
    }
    Node temp = findNodeLocation(map, keyElement);
    if (temp->key != keyElement) {
        return NULL;
    }
    return temp->data;
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
    if(!map || !keyElement){
        return MAP_NULL_ARGUMENT;
    }
    Node temp=findNodeLocation(map,keyElement);
    if(temp->key!=keyElement){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    temp->prev->next=temp->next;
    temp->next->prev=temp->prev;
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
    while(map->iterator->prev!=NULL){
        map->iterator=map->iterator->prev;
    }
    return map->iterator->key;
}


MapKeyElement mapGetNext(Map map){
    return map->iterator->next;
}


MapResult mapClear(Map map){
    map->iterator=mapGetFirst(map);
    while(map->iterator->next!=NULL){
        map->iterator=map->iterator->next;
        mapRemove(map, map->iterator->prev->key);
    }
    MapResult result=mapRemove(map,map->iterator->key);
    return result;
}