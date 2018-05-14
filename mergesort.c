#include "mergesort.h"

void mergeSort(Element a[], int n,CmpFunction compare);
void Msort(Element a[],int n, Element  helper_array[],CmpFunction compare);
void merge(Element a[], int na, Element b[], int nb, Element c[], CmpFunction compare);



void mergeSort(Element a[], int n,CmpFunction compare) {
    assert(a != NULL && compare != NULL);
    Element* tmp_array = malloc(sizeof(Element) * n);
    assert(tmp_array!=NULL);
    Msort(a, n, tmp_array,compare);
    free(tmp_array);
}
void Msort(Element a[], int n, Element helper_array[],CmpFunction compare)
{
    int left = n / 2;
    int right = n - left;
    if (n < 2)
        return;
    Msort(a, left, helper_array,compare);
    Msort(a + left, right, helper_array,compare);
    merge(a, left, a + left, right, helper_array,compare);
    memcpy(a, helper_array, n * sizeof(Element));//
}




void merge(Element a[], int na, Element b[], int nb, Element c[], CmpFunction compare) {//too long
    int ia, ib, ic;
    for(ia = ib = ic = 0; (ia < na) && (ib < nb); ic++) {
        if(compare(a[ia],b[ib])) {
            c[ic] = a[ia];
            ia++;
        }
        else {
            c[ic] = b[ib];
            ib++;
        }
    }
    for(;ia < na; ia++, ic++) c[ic] = a[ia];
    for(;ib < nb; ib++, ic++) c[ic] = b[ib];
}

