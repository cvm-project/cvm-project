#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    size_t oneHundredMiB = 100 * 1048576;
    size_t maxMemMiB = 0;
    void* memPointer = NULL;
    do {
        if (memPointer != NULL) {
            printf("Max Tested Memory = %zi\n", maxMemMiB);
            memset(memPointer, 0, maxMemMiB);
            free(memPointer);
        }
        maxMemMiB += oneHundredMiB;
        memPointer = malloc(maxMemMiB);
    } while (memPointer != NULL);
    printf("Max Usable Memory aprox = %zi\n", maxMemMiB - oneHundredMiB);

    return 0;
}
