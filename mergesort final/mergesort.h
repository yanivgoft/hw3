#ifndef DRY_MERGESORT_LIBRARY_H
#define DRY_MERGESORT_LIBRARY_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef void* Element;
typedef bool (*CmpFunction) (Element e1,Element e2);
void mergeSort(Element* a, int n,CmpFunction compare);
void Msort(Element* a,int n, Element*  helper_array,CmpFunction compare);
void merge(Element* a, int na, Element* b, int nb, Element* c, CmpFunction compare);


#endif