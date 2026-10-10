#include <time.h>
#include "miller-rabin.h"

typedef unsigned long long ull;

ull extGCD_inverse(ull a, ull m) {
    long long m0 = m;
    long long y = 0, x = 1;
    long long q, t;
    long long signed_a = a;

    if (m == 1) return 0;

    while (signed_a > 1) {
        q = signed_a / (long long)m;
        t = m;

        m = signed_a % m;
        signed_a = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < 0) x += m0;

    return (ull)x;
}

ull encode(ull m, ull e, ull N) {
    return powmod(m, e, N);
}

ull decode(ull E, ull d, ull N) {
    return powmod(E, d, N);
}

int main(void) {
    srand((unsigned int)time(NULL));

    // Notice: Primes for RSA are typically odd, so we can search in an appropriate range
    ull min = 1ULL << 30;
    ull max = 1ULL << 32;

    ull p = get_random_prime(min, max);
    ull q = get_random_prime(min, max);
    printf("p = %llu\n", p);
    printf("q = %llu\n", q);
    
    ull N = p * q;
    printf("N = %llu\n", N);

    ull r = (p-1) * (q-1);
    ull e = (1U << 16) + 1;
    ull d = extGCD_inverse(e, r);

    if (mulmod(e, d, r) == 1) {
        printf("Valid: e * d == 1 mod r\n");
    } else {
        printf("Invalid: They are not modular inverses.\n");
    }

    ull m = 128372193;
    ull E = encode(m, e, N);
    ull m_dash = decode(E, d, N);

    printf("%llu, %llu, %llu", m, E, m_dash);
    return 0;
}
