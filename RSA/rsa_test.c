#include <time.h>
#include "miller-rabin.h"


int main(void) {
    srand((unsigned int)time(NULL));

    // Notice: Primes for RSA are typically odd, so we can search in an appropriate range
    unsigned long long min = 100000000000ULL;  // 100 Billion
    unsigned long long max = 999999999999ULL;  // ~1 Trillion

    printf("Generating 3 large random prime numbers:\n");
    for (int i = 0; i < 3; i++) {
        printf("Prime %d: %llu\n", i + 1, get_random_prime(min, max));
    }

    return 0;
}
