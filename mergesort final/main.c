#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "mergesort.h"

bool intIsBigger(Element a,Element b);

int main()
{

   int* numbers[4];
   numbers[0] = malloc(sizeof(int));
   *numbers[0] = 17;
   numbers[1] = malloc(sizeof(int));
   *numbers[1] = 1;
   numbers[2] = malloc(sizeof(int));
   *numbers[2]=3;
   mergeSort((void**)numbers,3,intIsBigger);
   for(int i=0;i<3;i++){
      printf("%d \n",*numbers[i]);
   }
   return 0;

}
bool intIsBigger(Element a,Element b){
    return ( *(int*) a ) > ( *(int*) b );

}