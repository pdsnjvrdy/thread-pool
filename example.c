#include "pool.h"
#include <stdio.h>
#include <unistd.h> 

// Sample task function
void print_number(void *arg) {
    int num = *(int*)arg;
    printf("Task %d running\n", num);
    // Simulate a tiny bit of work
    usleep(10000);
}

int main() {
    thread_pool_t *pool = pool_create(4);
    if (pool == NULL) {
        printf("pool create failed\n");
        return 1;
    }

    int nums[8] = {1,2,3,4,5,6,7,8};

    // Submit 8 tasks
    for (int i = 0; i < 8; i++) {
        pool_submit(pool, print_number, &nums[i]);
    }

    printf("All tasks submitted. Waiting...\n");

    // Give workers time to process
    sleep(1);

    pool_destroy(pool);
    printf("Pool destroyed.\n");

    return 0;
}