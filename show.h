#ifndef SHOW_H_
#define SHOW_H_

#include "mtm_ex3.h"
typedef void* Element;
typedef struct show_t  {
    int ages[2];
    int episode_duration;
    char* name;
    int episode_num;
    Genre genre;
}*Show;



Show showCreate(char* name, Genre genre, int ages[2], int episode_duration,
                int episode_num);
void showDestroy(Element show);
Element showCopy(Element input);
#endif