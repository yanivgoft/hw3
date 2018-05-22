#ifndef USER_H_
#define USER_H_
#include "set.h"
#ifndef ELEMENT_
#define ELEMENT_
typedef void* Element;
#endif
typedef struct user_t  {
    int age;
    char* user_name;
    Set fav_shows;
    Set friends;

}*User;
User userCreate(const char* new_name,int age);
void userDestroy(Element input);
Element userCopy(Element input);
Element stringCopy(Element input);
int stringCompare(Element e1, Element e2);
void stringDestroy(Element input);

#endif