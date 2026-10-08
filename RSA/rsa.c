#include "miller-rabin.h"
#include <stdlib.h>
#include <time.h>


int randint(int min, int max) {
    if (max > RAND_MAX) {
        printf("Max is higher than RAND_MAX\n");
    }
    return (rand() % (max - min + 1)) + min;
}

int main(void) {
    srand(time(NULL) * 1234);
    int is_prime = false;
    do {
        int n = randint(1, 10000);
        is_prime = miller_rabin_deterministic(n);
        printf("Random number: %d\n", n);
        printf("Is prime (1=Yes, 0=No): %d\n", is_prime);
    } while (!is_prime);
    return 0;
}