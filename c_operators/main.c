#include <stdio.h>
#include <stdlib.h>
#include "intersect.h"

int main(void)
{
    int left[6] = {1,2,3,4,5, 6};
    int right[3] = {4,5,6};
    int res_size;
    int* result = intersect(left, right, 6, 3, &res_size);

    for (int i = 0; i < res_size; i++){
        printf("%d\n", result[i]);
    }
    return 0;
}