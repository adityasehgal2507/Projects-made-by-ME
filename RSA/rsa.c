#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "miller-rabin.h"

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
        return random_uint64();
    }

    return min + (random_uint64() % range);
}

// Generates a random prime number within a specific range [min, max]
unsigned long long get_random_prime(unsigned long long min, unsigned long long max) {
    while (1) {
        unsigned long long candidate = randint_large(min, max);
        
        // Optimisation: If it's even and not 2, it's not prime. Skip the heavy test.
        if (candidate % 2 == 0 && candidate != 2) {
            continue; 
        }
        
        // Ensure 0 and 1 are not treated as prime
        if (candidate < 2) {
            continue;
        }

        // Call the Miller-Rabin function from your header file. 
        // 10 to 20 rounds (k) is usually standard for strong accuracy.
        if (miller_rabin_deterministic(candidate)) { 
            return candidate;
        }
    }
}

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
