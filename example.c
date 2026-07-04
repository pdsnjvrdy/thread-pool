#include "pool.h"
#include <stdio.h>

int main() {
    thread_pool_t *pool = pool_create(4);
    if (pool == NULL) {
        printf("pool creation failed\n");
        return 1;
    }

    printf("Pool created with 4 threads.\n");

    pool_destroy(pool);
    printf("Pool destroyed (sort of).\n");

    return 0;
}