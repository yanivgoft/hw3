#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "show.h"
#include "C:\Users\yaniv\Desktop\mtm\hw3\HW3_Files_GoodLuck_v3\mtm_ex3.h"


const char* genres[] = { "SCIENCE_FICTION", "DRAMA", "COMEDY", "CRIME",
                         "MYSTERY", "DOCUMENTARY", "ROMANCE", "HORROR"};


Show showCreate(char* name, Genre genre, int ages[2], int episode_duration,
                int episode_num) {
    Show show = malloc(sizeof(*show));
    if (!show) {
        return NULL;
    }

    show->name = malloc(sizeof(char) * (strlen(name) + 1));
    if (!show->name) {
        free(show);
        return NULL;
    }
    show->genre = genre;
    strcpy(show->name, name);
    show->ages[0] = ages[0];
    show->ages[1] = ages[1];
    if (ages[0] < MTM_MIN_AGE) {
        show->ages[0] = MTM_MIN_AGE;
    }
    if (ages[1] > MTM_MAX_AGE) {
        show->ages[1] = MTM_MAX_AGE;
    }
    show->episode_num=episode_num;
    show->episode_duration=episode_duration;
    return show;
}

void showDestroy(Element show){
    assert(show);
    Show my_show=(Show)show;
    free(my_show->name);
    free(my_show);
    my_show=NULL;
}

Element showCopy(Element input){
    Show show=(Show)input;
    Show copy=showCreate(show->name,show->genre,show->ages,
                    show->episode_duration, show->episode_num);
    if(!copy){
        return NULL;
    }
    return copy;
}

int showCompare(Element e1, Element e2){
    assert(e1 && e2);
    Show show1=(Show )e1;
    Show show2=(Show)e2;
    int compare=strcmp(genres[show1->genre], genres[show2->genre]);
    if(compare!=0){
        return compare;
    }
    return strcmp(show1->name,show2->name);
}