#include <stdlib.h>
#include "intersect.h"

int* intersect (int* left, int* right, int n, int m, int* result_size) {
    int* ret = (int*) malloc(sizeof(int)*(n < m ? n : m));
    int ret_counter = 0;
    for (int i = 0; i < n; i++){
        for (int j = 0;j < m;  j++){
            if (left[i] == right[j]) {
                ret[ret_counter++] = left[i];
                break;
            }
        }
    }
    ret = (int*) realloc(ret, ret_counter+1);
    *result_size = ret_counter;
    return ret;
}