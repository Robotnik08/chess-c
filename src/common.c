#include "common.h"

void printBits(unsigned long long int n) {
    for (int i = 63; i >= 0; i--) {
        putchar((n & (1ULL << i)) ? '1' : '0');
        if (i % 8 == 0) putchar(' ');
    }
    putchar('\n');
}

int random_counter = 0;

long long int pseudoRandom(long long int seed) {
    // A simple linear congruential generator (LCG)
    return (seed * 6364136223846793005LL + 1) & 0xFFFFFFFFFFFFFFFFLL;
}

long long int random64(long long int seed) {
    // Generate a 64-bit random number using LCG
    long long int result = 0;
    for (int i = 0; i < 64; i += 8) {
        result |= (pseudoRandom(seed + random_counter) & 0xFF) << i;
        seed++;
    }
    random_counter++;
    return result;
}