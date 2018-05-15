#ifndef USER_H_
#define USER_H_
#include "set.h"
typedef void* Element;
typedef struct user_t  {
    int age;
    char* user_name;
    Set fav_shows;
    Set friends;

}*User;
User userCreate(const char* new_name,int age);
void userDestroy(Element input);
Element userCopy(Element input);
int userCompare(Element e1,Element e2);

#endif