#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Generates a 64-bit random unsigned integer
unsigned long long random_uint64() {
    return ((unsigned long long)rand() << 48) |
           ((unsigned long long)rand() << 32) |
           ((unsigned long long)rand() << 16) |
           ((unsigned long long)rand());
}

// Generates a large random number within a specific range [min, max]
unsigned long long randint_large(unsigned long long min, unsigned long long max) {
    if (min > max)
        return 0;

    unsigned long long range = max - min + 1;
    if (range == 0) {
        // Handles the edge case where the range is exactly 2^64
        return random_uint64();
    }

    return min + (random_uint64() % range);
}

int main(void) {
    srand((unsigned int)time(NULL));

    unsigned long long min = 100000000000ULL;  // 100 Billion
    unsigned long long max = 999999999999ULL;  // ~1 Trillion

    printf("Generating 5 large random numbers:\n");
    for (int i = 0; i < 5; i++) {
        printf("%llu\n", randint_large(min, max));
    }

    return 0;
}
