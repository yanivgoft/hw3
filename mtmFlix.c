#include <stdlib.h>
#include <string.h>
#include "mtmflix.h"
#include "set.h"
#include "show.h"
#include "user.h"
#include <assert.h>
#include "map.h"
#include "list.h"

static int countFavoriteGenre(Map shows, Set fav_shows, Genre genre);
static void removeElementFromUsers(Map map, char* name, void (*remove)(User user, void* name));
static double calculateAvgDuration(Map shows, Set fav_show);
static int countFriendLikes(MtmFlix mtmflix, Set friends, char* show);
static int calculateShowGrade(int friends, int duration,
                              double avg_duration, int genre);
int indexOfMaxGradedShow(MtmFlix mtmflix, Show* show_arr, Set friends, int n,
                                Set fav_shows, double avg_duration);
void removeFavShow(User user, void* name);
void removeFriends(User user, void* name);
static void sortRecomnedShow(MtmFlix mtmflix, Show* show_arr, int n, Set friends,
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
    if(mtmflix) {
        mapDestroy(mtmflix->users);
        mapDestroy(mtmflix->shows);
        free(mtmflix);
        mtmflix = NULL;
    }
}

MtmFlixResult mtmFlixAddUser(MtmFlix mtmflix, const char* username, int age) {
    if(mtmflix==NULL || username==NULL){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(*username=='\0'){
        return MTMFLIX_ILLEGAL_USERNAME;
    }
    if(mapContains(mtmflix->users, (void*)username)){
        return MTMFLIX_USERNAME_ALREADY_USED;
    }
    if(age<MTM_MIN_AGE || age>MTM_MAX_AGE){
        return MTMFLIX_ILLEGAL_AGE;
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
    if(!mtmflix || !username){
        return MTMFLIX_NULL_ARGUMENT;
    }
    MapResult result=mapRemove(mtmflix->users,(void*)username);
    if(result==MAP_NULL_ARGUMENT){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(result==MAP_ITEM_DOES_NOT_EXIST){
        return  MTMFLIX_USER_DOES_NOT_EXIST;
    }
    removeElementFromUsers(mtmflix->users, (char*)username, removeFriends);
    return MTMFLIX_SUCCESS;
}

MtmFlixResult mtmFlixAddFriend(MtmFlix mtmflix, const char* username1,
                               const char* username2){
    if(!mtmflix || !username1 || !username2){
        return MTMFLIX_NULL_ARGUMENT;
    }
    if(strcmp(username1,username2)==0){
        return MTMFLIX_ILLEGAL_USERNAME;
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
    if(mapContains(mtmflix->shows, (void*)name)){
        return MTMFLIX_SERIES_ALREADY_EXISTS;
    }
    if(episodesNum<=0){
        return MTMFLIX_ILLEGAL_EPISODES_NUM;
    }
    if(episodesDuration<=0){
        return MTMFLIX_ILLEGAL_EPISODES_DURATION;
    }
    int ages2[2];
    if(!ages){
        ages=ages2;
        ages[0]=MTM_MIN_AGE;
        ages[1]=MTM_MAX_AGE;
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

void removeFavShow(User user, void* name){
    setRemove(user->fav_shows, name);
}
void removeFriends(User user, void* name){
    setRemove(user->friends, name);
}

static void removeElementFromUsers(Map map, char* name, void (*remove)(User user, void* name)){
    char* username=mapGetFirst(map);
    while(username!=NULL){
        User user=mapGet(map,username);
        remove(user,(void*)name);
        username=mapGetNext(map);

    }
}

bool legalName(char *name) {
    if(!name ||*name =='\0'){
        return false;
    }
    for (int i = 0; i < strlen(name); i++) {
        if(!((name[i]>='a' && name[i]<='z') ||
             (name[i]>='A' && name[i]<='Z') ||
             (name[i]>='0' && name[i]<='9'))) {
            return false;
        }
    }
    return true;
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
    removeElementFromUsers(mtmflix->users, (char*)name, removeFavShow);
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
        return MTMFLIX_USER_NOT_IN_THE_RIGHT_AGE;
    }
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
        int compare=strcmp(genres[show_arr[i]->genre], genres[show_arr[i_max]->genre]);
        if(compare>0) {
            i_max = i;
        }
        else if(compare==0){
            if(strcmp(show_arr[i]->name,show_arr[i_max]->name)>0){
                i_max=i;
            }
        }
    }
    return i_max;
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
    assert(shows && n>0);
    Show* show_arr= malloc(sizeof(*show_arr)*n);
    if (!show_arr) {
        return NULL;
    }
    if(n!=0) {
        show_arr[0] = (Show) mapGet(shows, mapGetFirst(shows));
    }
    for (int i = 1; i < n; i++) {
        show_arr[i] = (Show) mapGet(shows, mapGetNext(shows));
    }
    return show_arr;
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
    sortArr((void **) show_arr, n, indexOfMaxShow);
    for (int i = 0; i < mapGetSize(mtmflix->shows); i++) {
        fprintf(outputStream, "%s\n", mtmPrintSeries(show_arr[i]->name, genres[show_arr[i]->genre]));
    }
    free(show_arr);//This is only a pointer array so it was mallocated without a create function
    //Do we need Fclose here or in main func?
    //P.s whtat's the meaning of life?
    return MTMFLIX_SUCCESS;
}

static List setToList(Set set){
    List list=listCreate(stringCopy,stringDestroy);
    if(!list){
        return NULL;
    }
    char* name=(char*)setGetFirst(set);
    if(!name) {
        return list;
    }
    listInsertFirst(list, (void*)name);
    for (int i = 1; i < setGetSize(set); i++) {
        name=setGetNext(set);
        listInsertLast(list,name);
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
    User* user_arr=malloc(sizeof(User)*n);
    if(!user_arr){
        return  MTMFLIX_OUT_OF_MEMORY;
    }
    user_arr[0]=mapGet(mtmflix->users,mapGetFirst(mtmflix->users));
    for (int i = 1; i < n; i++) {
        user_arr[i]=mapGet(mtmflix->users,mapGetNext(mtmflix->users));
    }
    sortArr((void**)user_arr, n, indexOfMaxUser);
    for (int i = 0; i < n; i++) {
        List friends_list = setToList(user_arr[i]->friends);
        List show_list = setToList(user_arr[i]->fav_shows);
        char *to_print = (char *) mtmPrintUser(user_arr[i]->user_name,
                                               user_arr[i]->age, friends_list,show_list);
        if (!to_print) {
            listDestroy(friends_list);
            listDestroy(show_list);
            return MTMFLIX_OUT_OF_MEMORY;
        }
        fprintf(outputStream, to_print);
        listDestroy(friends_list);
        listDestroy(show_list);
    }
    free(user_arr);
    return MTMFLIX_SUCCESS;
}

static Map createSubMap(MtmFlix mtmflix, char* username){
    Map new_map=mapCopy(mtmflix->shows);
    Map temp=mapCopy(mtmflix->shows);
    User user=mapGet(mtmflix->users,username);
    char* show_name=mapGetFirst(mtmflix->shows);
    while(show_name) {
        Show show = mapGet(temp, show_name);
        if((setIsIn(user->fav_shows,(void*)show_name))||user->age<show->ages[0]
           || user->age>show->ages[1] || setGetSize(user->friends)==0) {
            mapRemove(new_map, (void *) show_name);
        }
        else if(calculateShowGrade(countFriendLikes(mtmflix, user->friends,
                       show->name), show->episode_duration,calculateAvgDuration
                       (temp,user->fav_shows),countFavoriteGenre(temp,user->fav_shows,show->genre))==0) {
            mapRemove(new_map, (void *) show_name);
        }
        show_name=(char*)mapGetNext(mtmflix->shows);
    }
    mapDestroy(temp);
    return new_map;
}

static double calculateAvgDuration(Map shows, Set fav_show){
    assert(shows&& fav_show);
    if(setGetSize(fav_show)==0){
        return 0;
    }
    Show show=mapGet(shows, setGetFirst(fav_show));
    double sum=show->episode_duration;
    for (int i = 1; i < setGetSize(fav_show); i++) {
        show=mapGet(shows,setGetNext(fav_show));
        sum+=show->episode_duration;
    }
    sum/=setGetSize(fav_show);
    return sum;
}

static int countFriendLikes(MtmFlix mtmflix, Set friends, char* show){
    assert(friends && show);
    User friend=mapGet(mtmflix->users,setGetFirst(friends));
    int counter=setIsIn(friend->fav_shows,(void*)show);
    for (int i = 0; i <setGetSize(friends)-1 ; i++) {
        friend = mapGet(mtmflix->users, setGetNext(friends));
        if (setIsIn(friend->fav_shows, (void *) show)) {
            counter++;
        }
    }
    return counter;
}

static int countFavoriteGenre(Map shows, Set fav_shows, Genre genre){
    assert(fav_shows);
    Show show=mapGet(shows,setGetFirst(fav_shows));
    int counter=(show->genre==genre);
    for (int i = 1; i < setGetSize(fav_shows); i++) {
        show=mapGet(shows,setGetNext(fav_shows));
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

static void sortRecomnedShow(MtmFlix mtmflix, Show* show_arr, int n, Set friends,
                             Set fav_shows, double avg_duration) {
    int i_max;
    for (int length = n; length > 1; length--) {
        i_max = indexOfMaxGradedShow(mtmflix, show_arr, friends, length, fav_shows, avg_duration);
        void *temp = show_arr[i_max];
        show_arr[i_max] = show_arr[length - 1];
        show_arr[length - 1] = temp;
    }
}
int indexOfMaxGradedShow(MtmFlix mtmflix, Show* show_arr, Set friends, int n,
                                Set fav_shows, double avg_duration){
    int i_max=0;
    int friendsLike=countFriendLikes(mtmflix, friends,show_arr[0]->name);
    int genre=countFavoriteGenre(mtmflix->shows,fav_shows,show_arr[0]->genre);
    int min_grade=calculateShowGrade(friendsLike,
             show_arr[0]->episode_duration,avg_duration,genre);
    for (int i = 1; i < n; i++) {
        friendsLike=countFriendLikes(mtmflix, friends,show_arr[i]->name);
        genre=countFavoriteGenre(mtmflix->shows,fav_shows,show_arr[i]->genre);
        int temp_grade=calculateShowGrade(friendsLike,
                show_arr[i]->episode_duration,avg_duration, genre);

        if(temp_grade<min_grade) {
            i_max = i;
            min_grade=temp_grade;
        }
        else if(temp_grade==min_grade &&
                strcmp(show_arr[i]->name,show_arr[i_max]->name)>0) {
            i_max = i;
        }
    }
    return i_max;
}


MtmFlixResult mtmFlixGetRecommendations(MtmFlix mtmflix,const char* username,
                                        int count, FILE* outputStream){
    if(!mtmflix || !username || !outputStream){
        return  MTMFLIX_NULL_ARGUMENT;
    }
    if(!mapContains((mtmflix->users), (char*)username)){
        return MTMFLIX_USER_DOES_NOT_EXIST;
    }
    if(count<0){
        return MTMFLIX_ILLEGAL_NUMBER;
    }
    Map new_map=createSubMap(mtmflix,(char*)username);
    User user=mapGet(mtmflix->users,(void*)username);
    int avgDuration=calculateAvgDuration(mtmflix->shows,user->fav_shows);
    Show* show_arr= createShowArr(new_map,mapGetSize(new_map));
    sortRecomnedShow(mtmflix, show_arr, mapGetSize(new_map),user->friends,user->fav_shows,avgDuration);
    if(count==0 || count>mapGetSize(new_map)){
        count=mapGetSize(new_map);
    }
    for (int i = 0; i < count; i++) {
        fprintf(outputStream, mtmPrintSeries(show_arr[i]->name, genres[show_arr[i]->genre]));
    }
    mapDestroy(new_map);
    free(show_arr);
    return MTMFLIX_SUCCESS;
}

/*int main() {
    printf("Hello, World!\n");
    return 0;
}*/