#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mtmflix.h"
#include "map.h"
#include "set.h"
#include "show.h"
#include "user.h"
#include <stdbool.h>
#include <assert.h>

static void removeElementFromUsers(Map map, char* name, bool shows);
static double calculateAvgDuration(MtmFlix mtmflix, Set fav_show);
static int countFriendLikes(Set friends, char* show);
static int calculateShowGrade(int friends, int duration,
                              double avg_duration, int genre);
static int indexOfMaxGradedShow(Show* show_arr, Set friends, int n,
                                Set fav_shows, double avg_duration);

char* genres[] = { "SCIENCE_FICTION", "DRAMA", "COMEDY", "CRIME",
                   "MYSTERY", "DOCUMENTARY", "ROMANCE", "HORROR"};


static bool legalName(char* name);

struct mtmFlix_t{
    Map shows;
    Map users;
};


MtmFlix mtmFlixCreate(){
    MtmFlix new_flix=malloc(sizeof(*new_flix));
    if(!new_flix){
        return NULL;
    }
    new_flix->users=mapCreate(userCopy, stringCopy, userDestroy, stringDestroy, stringCompare);
    new_flix->shows=mapCreate(showCopy, stringCopy, showDestroy, stringDestroy, stringCompare);
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
    if(result==MAP_ITEM_DOES_NOT_EXIST){
        return  MTMFLIX_USER_DOES_NOT_EXIST;
    }
    removeElementFromUsers(mtmflix->users, (char*)username, false);
    return MTMFLIX_SUCCESS;
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
    User user=(mapGet(mtmflix->users, (void*)username1));
    SetResult result=setAdd(user->friends,(void*)username2);
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
    User user=mapGet(mtmflix->users, (void*)username1);
    setRemove(user->friends, (void*)username2);
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

static void removeElementFromUsers(Map map, char* name, bool shows){
    User user=mapGetFirst(map);
    while(user!=NULL){
        if(shows) {
            setRemove(user->fav_shows, (void *) name);
        }
        else{
            setRemove(user->friends, (void*)name);
        }
        user=mapGetNext(map);
    }
}

bool legalName(char *name) {
    if(!name ||name==""){
        return false;
    }
    for (int i = 0; i < strlen(name); i++) {
        if(!((name[i]>'a' && name[i]<'z') ||
             (name[i]>'A' && name[i]<'Z') ||
             (name[i]>'0' && name[i]<'9'))) {
            return false;
        }
        return true;
    }
}


MtmFlixResult mtmFlixRemoveSeries(MtmFlix mtmflix, const char* name){
    if(!mtmflix || !name){
        return  MTMFLIX_NULL_ARGUMENT;
    }
    MapResult result=mapRemove(mtmflix->shows,(void*)name);
    if(result==MAP_NULL_ARGUMENT){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(result==MAP_ITEM_DOES_NOT_EXIST){
        return MTMFLIX_SERIES_DOES_NOT_EXIST;
    }
    removeElementFromUsers(mtmflix->users, (char*)name, true);
    return MTMFLIX_SUCCESS;
}

MtmFlixResult mtmFlixSeriesJoin(MtmFlix mtmflix, const char* username,
                                const char* seriesName) {
    if (!mtmflix || !username || !seriesName) {
        return MTMFLIX_NULL_ARGUMENT;
    }
    if (!mapContains(mtmflix->users, (void *) username)) {
        return MTMFLIX_USER_DOES_NOT_EXIST;
    }
    if (!mapContains(mtmflix->shows, (void *) seriesName)) {
        return MTMFLIX_SERIES_DOES_NOT_EXIST;
    }
    Show show=mapGet(mtmflix->shows, (char*)seriesName);
    User user=mapGet(mtmflix->users, (char*)username);
    if(show->ages[0]>user->age || show->ages[1]<user->age){
        MTMFLIX_USER_NOT_IN_THE_RIGHT_AGE;
    }
    showDestroy(show);
    userDestroy(user);
    setAdd(user->fav_shows, (void*)seriesName);
    return MTMFLIX_SUCCESS;
}
MtmFlixResult mtmFlixSeriesLeave(MtmFlix mtmflix, const char* username,
                                 const char* seriesName){
    if(!mtmflix || !username || !seriesName){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(!mapContains(mtmflix->users, (void*)username)){
        return MTMFLIX_USER_DOES_NOT_EXIST;
    }
    if(!mapContains(mtmflix->shows, (void*)seriesName)){
        return MTMFLIX_SERIES_DOES_NOT_EXIST;
    }
    User user=mapGet(mtmflix->users, (void*)username);
    setRemove(user->fav_shows, (void*)seriesName);
    return MTMFLIX_SUCCESS;
}


static int indexOfMaxShow(void** arr, int n){
    Show* show_arr=(Show*)arr;
    int i_max=0;
    for (int i = 1; i < n; i++) {
        char* g1=(char*)genres[show_arr[i]->genre];
        char* g2=(char*)genres[show_arr[i_max]->genre];
        int compare=strcmp(g1, g2);
        if(compare>0){
            i_max=i;
        }
        else if(compare==0){
            if(strcmp(show_arr[i]->name,show_arr[i_max]->name)>0){
                i_max=i;
            }
        }
    }
}

static int indexOfMaxUser(void** arr,int n){
    User* user_arr=(User*)arr;
    int i_max=0;
    for(int i=1;i<n;i++){
        if(strcmp(user_arr[i]->user_name,user_arr[i_max]->user_name)>0){
            i_max=i;
        }
    }
    return i_max;
}



static void sortArr(void** arr, int n, int (*indexOfMax)(void**arr, int n)){
    int i_max;
    for (int length = n; length > 1 ; length--) {
            i_max = indexOfMax(arr, length);
        void* temp=arr[i_max];
        arr[i_max]=arr[length-1];
        arr[length-1]=temp;
    }
}

static Show* createShowArr(Map shows, int n){
    Show *show_arr = malloc(sizeof(show_arr)*n);
    if (!show_arr) {
        return NULL;
    }
    show_arr[0] = mapGetFirst(shows);
    for (int i = 1; i < n; i++) {
        show_arr[i] = mapGetNext(shows);
    }
}

MtmFlixResult mtmFlixReportSeries(MtmFlix mtmflix,
                                  int seriesNum, FILE* outputStream) {
    if (!mtmflix || !outputStream) {
        return MTMFLIX_NULL_ARGUMENT;
    }
    int n = mapGetSize(mtmflix->shows);
    if (n == 0) {
        return MTMFLIX_NO_SERIES;
    }
    Show *show_arr = createShowArr(mtmflix->shows,n);
    sortArr((void **) mtmflix->shows, n, indexOfMaxShow);
    for (int i = 0; i < mapGetSize(mtmflix->shows); i++) {
        fprintf(outputStream, mtmPrintSeries(show_arr[i]->name, genres[show_arr[i]->genre]));
    }
    free(show_arr);//This is only a pointer array so it was mallocated without a create function
    //Do we need Fclose here or in main func?
    //P.s whtat's the meaning of life?
    return MTMFLIX_SUCCESS;
}

static List setToList(Set set){
    List list=listCreate(stringCopy,stringDestroy);
    listInsertFirst(list, setGetFirst(set));
    for (int i = 0; i < setGetSize(set); i++) {
        listInsertAfterCurrent(list,setGetNext(set));
    }
    return list;
}

MtmFlixResult mtmFlixReportUsers(MtmFlix mtmflix, FILE* outputStream){
    if(!mtmflix || !outputStream){
        return MTMFLIX_NULL_ARGUMENT;
    }
    int n=mapGetSize(mtmflix->users);
    if(n==0){
        return MTMFLIX_NO_USERS;
    }
    User* user_arr=malloc(sizeof(user_arr));
    if(!user_arr){
        return  MTMFLIX_OUT_OF_MEMORY;
    }
    user_arr[0]=mapGetFirst(mtmflix->users);
    for (int i = 1; i < n; i++) {
        user_arr[i]=mapGetNext(mtmflix->users);
    }
    sortArr((void**)user_arr, n, indexOfMaxUser);
    for (int i = 0; i < mapGetSize(mtmflix->users); i++) {
        fprintf(outputStream,mtmPrintUser(user_arr[i]->user_name,
                        user_arr[i]->age, setToList(user_arr[i]->friends),
                 setToList(user_arr[i]->fav_shows)));

    }
    free(user_arr);
    return MTMFLIX_SUCCESS;
}

static Map createSubMap(MtmFlix mtmflix, char* username, int count){
    Map new_map=mapCreate(showCopy,stringCopy,showDestroy, stringDestroy, stringCompare);
    User user=mapGet(mtmflix->users,username);
    if(count==0 || count>mapGetSize(mtmflix->shows)-setGetSize(user->fav_shows)){
        count=mapGetSize(mtmflix->shows)-setGetSize(user->fav_shows);
    }
    Show show=mapGetFirst(mtmflix->shows);
    for (int i = 0; i < count; i++) {
        if((setIsIn(user->fav_shows, (void*)show->name))||
                user->age<show->ages[0] || user->age>show->ages[1] ||
                (calculateShowGrade(countFriendLikes(user->friends,show->name),
                                    show->episode_duration,
                                    calculateAvgDuration(mtmflix,user->fav_shows),
                                    show->genre)==0)){
            i--;
            show = mapGetNext(mtmflix->shows);
            continue;
        }
        mapPut(new_map, (void*)show->name, (void*)show);
        show=mapGetNext(mtmflix->shows);
    }
    userDestroy(user);
    return new_map;
}

static double calculateAvgDuration(MtmFlix mtmflix, Set fav_show){
    assert(mtmflix&& fav_show);
    Show show=mapGet(mtmflix->shows, setGetFirst(fav_show));
    double sum=show->episode_duration;
    for (int i = 0; i < setGetSize(fav_show); i++) {
        show=mapGet(mtmflix->shows,setGetNext(fav_show));
        sum+=show->episode_duration;
    }
    sum/=setGetSize(fav_show);
    return sum;
}

static int countFriendLikes(Set friends, char* show){
    assert(friends && show);
    User friend=setGetFirst(friends);
    int counter=setIsIn(friend->fav_shows, (void*)show);
    for (int i = 0; i < setGetSize(friends); i++) {
        friend=setGetNext(friends);
        if(setIsIn(friend->fav_shows,(void*)show)){
            counter++;
        }
    }
    return counter;
}

static int countFavoriteGenre(Set fav_shows, Genre genre){
    assert(fav_shows);
    Show show=setGetFirst(fav_shows);
    int counter=(show->genre==genre);
    for (int i = 0; i < setGetSize(fav_shows); i++) {
        show=setGetNext(fav_shows);
        if(show->genre==genre){
            counter++;
        }
    }
    return counter;
}

static double absoloute(double a){
    if(a<0){
        return -a;
    }
    return a;
}


static int calculateShowGrade(int friends, int duration,
                              double avg_duration, int genre){
    double grade=genre*friends/(1+absoloute(avg_duration-duration));
    return (int)grade;
}

static void sortRecomnedShow(Show* show_arr, int n, Set friends,
                             Set fav_shows, double avg_duration){
    int i_max;
    for (int length = n; length > 1 ; length--) {
        i_max = indexOfMaxGradedShow(show_arr, friends, length, fav_shows, avg_duration);
        void* temp=show_arr[i_max];
        show_arr[i_max]=show_arr[length-1];
        show_arr[length-1]=temp;

    }

static int indexOfMaxGradedShow(Show* show_arr, Set friends, int n,
                                Set fav_shows, double avg_duration){
    int i_max=0;
    int min_grade;
    for (int i = 1; i < n; i++) {
        int friendsLike=countFriendLikes(friends,show_arr[i]->name);
        int genre=countFavoriteGenre(fav_shows,show_arr[i]->genre);
        int temp_grade=calculateShowGrade(friendsLike,
                show_arr[i]->episode_duration,avg_duration, genre);

        if(temp_grade<min_grade) {
            i_max = i;
        }
        else if(temp_grade==min_grade &&
                strcmp(show_arr[i]->name,show_arr[i_max]->name)>0) {
            i_max = i;
        }
    }
}


MtmFlixResult mtmFlixGetRecommendations(MtmFlix mtmflix, char* username,
                                        int count, FILE* outputStream){
    if(!mtmflix || !username || !outputStream){
        return  MTMFLIX_NULL_ARGUMENT;
    }
    if(!mapContains((mtmflix->users), username)){
        return MTMFLIX_USER_DOES_NOT_EXIST;
    }
    if(count<0){
        return MTMFLIX_ILLEGAL_NUMBER;
    }
    Map new_map=createSubMap(mtmflix,username,count);
    User user=mapGet(mtmflix->users,username);
    int avgDuration=calculateAvgDuration(mtmflix,user->fav_shows);
    Show* show= createShowArr(new_map,mapGetSize(new_map));
    sortRecomnedShow(show, mapGetSize(new_map),user->friends,user->fav_shows,avgDuration);
    for (int i = 0; i < mapGetSize(new_map); i++) {
        fprintf(outputStream, mtmPrintSeries(show[i]->name, genres[show[i]->genre]));
    }
    free(show);
    return MTMFLIX_SUCCESS;
}

int main() {
    printf("Hello, World!\n");
    return 0;
}