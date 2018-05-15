#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mtmflix.h"
#include "map_mtm.h"
#include "set.h"
#include "show.h"
#include "user.h"
#include <stdbool.h>

static bool legalName(char* name){
    for (int i = 0; i < strlen(name); ++i) {
        if(!((name[i]>'a' && name[i]<'z') ||
           (name[i]>'A' && name[i]<'Z') ||
           (name[i]>'0' && name[i]<'9'))) {
            return false;
        }
        return true;
    }
}
struct mtmFlix_t{
    Map shows;
    Map users;
};


MtmFlix mtmFlixCreate(){
    MtmFlix new_flix=malloc(sizeof(*new_flix));
    if(!new_flix){
        return NULL;
    }
    return new_flix;
}
void mtmFlixDestroy(MtmFlix mtmflix){
    mapDestroy(mtmflix->users);
    mapDestroy(mtmflix->shows);
    free(mtmflix);
    mtmflix=NULL;
}

MtmFlixResult mtmFlixAddUser(MtmFlix mtmflix, const char* username, int age) {
    if(mtmflix==NULL || username==NULL){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(username==""){
        return MTMFLIX_ILLEGAL_USERNAME;
    }
    if(age<MTM_MIN_AGE || age>MTM_MAX_AGE){
        return MTMFLIX_ILLEGAL_AGE;
    }
    if(mapContains(mtmflix->users, (void*)username)){
        return MTMFLIX_USERNAME_ALREADY_USED;
    }
    if(!legalName((char*)username)){
        return MTMFLIX_ILLEGAL_USERNAME;
    }
    User user=userCreate(username, age);
    if(!user){
        return MTMFLIX_OUT_OF_MEMORY;
    }
    mapPut(mtmflix->users,(void*)username, (void*)user);
    userDestroy(user);
    user=NULL;
    return MTMFLIX_SUCCESS;
}


MtmFlixResult mtmFlixRemoveUser(MtmFlix mtmflix, const char* username){
    MapResult result=mapRemove(mtmflix->users,(void*)username);
    if(result==MAP_NULL_ARGUMENT){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(result==MAP_SUCCESS){
        return  MTMFLIX_SUCCESS;
    }
    return MTMFLIX_USER_DOES_NOT_EXIST;
}

MtmFlixResult mtmFlixAddFriend(MtmFlix mtmflix, const char* username1,
                               const char* username2){
    if(!mtmflix || !username1 || !username2){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(!mapContains(mtmflix->users, (void*)username1) ||
            !mapContains(mtmflix->users, (void*)username2)){
        return MTMFLIX_USER_DOES_NOT_EXIST;
    }
    SetResult result=setAdd(mapGet(mtmflix->users, (void*)username1),
                            (void*)username2);
    if(result==SET_OUT_OF_MEMORY){
        return MTMFLIX_OUT_OF_MEMORY;
    }
    return MTMFLIX_SUCCESS;
}

MtmFlixResult mtmFlixRemoveFriend(MtmFlix mtmflix, const char* username1,
                                  const char* username2){
    if(!mtmflix || !username1 || !username2){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(!mapContains(mtmflix->users, (void*)username1) ||
       !mapContains(mtmflix->users, (void*)username2)){
        return MTMFLIX_USER_DOES_NOT_EXIST;
    }
    setRemove(mapGet(mtmflix->users, (void*)username1), (void*)username2);
    return MTMFLIX_SUCCESS;
}

MtmFlixResult mtmFlixAddSeries(MtmFlix mtmflix, const char* name,
                               int episodesNum, Genre genre,
                               int* ages, int episodesDuration){
    if(!mtmflix || !name){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(!legalName((char*)name)){
        return MTMFLIX_ILLEGAL_SERIES_NAME;
    }
    if(episodesDuration<=0){
        return MTMFLIX_ILLEGAL_EPISODES_DURATION;
    }
    if(episodesNum<=0){
        return MTMFLIX_ILLEGAL_EPISODES_NUM;
    }
    if(mapContains(mtmflix->shows, (void*)name)){
        return MTMFLIX_SERIES_ALREADY_EXISTS;
    }
    Show show=showCreate((char*)name,genre,ages, episodesDuration, episodesNum);
    if(!show){
        return MTMFLIX_OUT_OF_MEMORY;
    }
    MapResult result=mapPut(mtmflix->shows,(void*)name,(void*)show);
    if(result==MAP_OUT_OF_MEMORY){
        return MTMFLIX_OUT_OF_MEMORY;
    }
    showDestroy(show);
    show=NULL;
    return MTMFLIX_SUCCESS;
}

MtmFlixResult mtmFlixRemoveSeries(MtmFlix mtmflix, const char* name){
    SetResult result=setRemove((void*)mapGet(mtmflix->shows, (void*)name),(void*)name);
    if(result==SET_NULL_ARGUMENT){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(result==SET_ITEM_DOES_NOT_EXIST){
        return MTMFLIX_SERIES_DOES_NOT_EXIST;
    }
    return MTMFLIX_SUCCESS;

}

int main() {
    printf("Hello, World!\n");
    return 0;
}