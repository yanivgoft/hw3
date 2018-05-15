#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "set.h"
#include "mtm_ex3.h"
#include "user.h"
#include "show.h"
typedef void* Element;

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
    return strcmp(e1,e2);
}
void stringDestroy(Element input){
    free(input);
    input=NULL;
}





/*
char* userGetName(User user){
    char* name=malloc(sizeof(strlen(user->user_name)+1));
    if(name==NULL){
        return NULL;
    }
    name[strlen(user->user_name)]='\0';
    return name;
}

int userGetAge(User user){
    return user->age;
}

void userSetAge(User user,int age){
    user->age=age;
}

Set userGetFavShows(User user){
    return  user->fav_shows;
}

Set userGetFriends(User user){
    return  user->friends;
}

bool userAddShow(User user,Show show){
    setAdd(user->fav_shows,show);
}

bool userAddFriend(User user,User friend){
    setAdd(user->friends,friend);
}
*/