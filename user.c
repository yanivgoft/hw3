#include <stdlib.h>
#include <string.h>
#include "user.h"
void userDestroy(Element input);
Element userCopy(Element input);
int userCompare(Element e1,Element e2);


User userCreate(const char* new_name,int age){
    User new_user=malloc(sizeof(*new_user));
    if(new_user==NULL){
        return NULL;
    }
    new_user->age=age;
    new_user->user_name=malloc(sizeof(char)*(strlen(new_name)+1));
    if(new_user->user_name==NULL){
        free(new_user);
        return NULL;
    }
    strcpy(new_user->user_name,new_name);
    new_user->fav_shows=setCreate(stringCopy, stringDestroy, stringCompare);
    if(!new_user->fav_shows){
        free(new_user->user_name);
        free(new_user);
        return NULL;
    }
    new_user->friends=setCreate(stringCopy, stringDestroy, stringCompare);
    if(!new_user->friends){
        setDestroy(new_user->fav_shows);
        free(new_user->user_name);
        free(new_user);
        return NULL;
    }
    return new_user;
}
void userDestroy(Element input){
    User user=(User)input;
    setDestroy(user->fav_shows);
    setDestroy(user->friends);
    free(user->user_name);
    free(user);
    user=NULL;
}


Element userCopy(Element input){
    User user=(User) input;
    User new_user=userCreate(user->user_name,user->age);
    if(new_user==NULL){
        return NULL;
    }
    setDestroy(new_user->fav_shows);
    setDestroy(new_user->friends);
    new_user->fav_shows=setCopy(user->fav_shows);
    new_user->friends=setCopy(user->friends);
    return new_user;
}

Element stringCopy(Element input){
    char* str=malloc(sizeof(char)*(strlen(input)+1));
    if(!str){
        return NULL;
    }
    strcpy(str,input);
    return str;
}
int stringCompare(Element e1, Element e2){
    return strcmp((char*)e1,(char*)e2);
}
void stringDestroy(Element input){
    free(input);
    input=NULL;
}
