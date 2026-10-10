#include <stdio.h>
#include <stdbool.h>
#include "miller-rabin.h"

typedef unsigned long long ull;

// Fixed: Bypasses the loop entirely using 128-bit native hardware math
ull mulmod(ull a, ull b, ull mod) {
    return ((unsigned __int128)a * b) % mod;
}

ull powmod(ull base, ull exp, ull mod) {
    ull result = 1;
    base = base % mod; 
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = mulmod(result, base, mod);
        }
        exp = exp / 2;
        base = mulmod(base, base, mod);
    }
    return result;
}

bool miller_rabin_deterministic(ull n) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    ull u = n - 1;
    int t = 0;
    while (u % 2 == 0) {
        u /= 2;
        t++;
    }

    // The 12 proven bases for 100% deterministic 64-bit accuracy
    // const ull bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    const ull bases [] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    const int num_bases = sizeof(bases) / sizeof(bases[0]);

    for (int i = 0; i < num_bases; i++) {
        ull a = bases[i];
        
        if (a >= n) {
            if (n == a) return true;
            continue;
        }

        ull v = powmod(a, u, n);
        
        if (v == 1 || v == n - 1) {
            continue;
        }

        int s;
        for (s = 0; s < t; s++) {
            v = mulmod(v, v, n);
            if (v == n - 1) {
                break;
            }
            if (v == 1) {        
                return false; 
            }
        }

        if (s == t) {
            return false;
        }
    }
    return true;
}

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

// int main() {
//     ull x = 13;
//     printf("Is prime: %d\n", miller_rabin_deterministic(x)); // Outputs 1 instantly

//     return 0;
// }

// int main() {
//     // Structure to hold our test data
//     struct TestCase {
//         ull number;
//         bool expected;
//         const char* description;
//     };

//     struct TestCase tests[] = {
//         // --- Trivial & Edge Cases ---
//         {0, false, "Zero"},
//         {1, false, "One"},
//         {2, true,  "Smallest Prime"},
//         {3, true,  "Small Odd Prime"},
//         {4, false, "Small Composite Even"},
//         {13, true, "Small Prime (Base divisor)"},
        
//         // --- Base Set Values ---
//         {325, false, "Base element itself (Composite)"},
        
//         // --- Strong Pseudoprimes / Pseudoprimes ---
//         {2047, false, "Strong pseudoprime to base 2 (23 * 89)"},
//         {1373653, false, "Strong pseudoprime to bases 2, 3 (829 * 1657)"},
//         {46856248657ULL, false, "Pseudoprime that fails if base 2 is missing"},
//         {3215031751ULL, false, "Mersenne-like composite (151 * 751 * 28351)"},
        
//         // --- Large 64-Bit Boundaries ---
//         {4294967291ULL, true, "Largest prime fit for 32-bit uint"},
//         {4294967297ULL, false, "Fermat Composite 2^32 + 1 (641 * 6700417)"},
//         {18446744073709551557ULL, true, "Largest prime below 2^64 (Checks overflow)"},
//         {18446744073709551615ULL, false, "Max 64-bit integer (2^64 - 1, Composite)"}
//     };

//     int total_tests = sizeof(tests) / sizeof(tests[0]);
//     int passed = 0;

//     printf("=== RUNNING MILLER-RABIN DETERMINISTIC TESTS ===\n\n");

//     for (int i = 0; i < total_tests; i++) {
//         bool result = miller_rabin_deterministic(tests[i].number);
//         bool match = (result == tests[i].expected);
        
//         if (match) {
//             printf("[ PASS ] ");
//             passed++;
//         } else {
//             printf("[ FAIL ] ");
//         }
        
//         printf("%-45s | Input: %-20llu | Expected: %d | Got: %d\n", 
//                tests[i].description, tests[i].number, tests[i].expected, result);
//     }

//     printf("\n=== TEST SUMMARY ===\n");
//     printf("Passed: %d / %d\n", passed, total_tests);
    
//     if (passed == total_tests) {
//         printf("Result: SUCCESS! Your 64-bit math and logic are flawless.\n");
//     } else {
//         printf("Result: FAILURE. Check for modular overflow or base logic flaws.\n");
//     }

//     return 0;
// }
