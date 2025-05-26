#include "common.h"


int countBits(unsigned long long int n) {
    int count = 0;
    for (; n; count++) n &= (n - 1);
    return count;
}

int countTrailingZeros(unsigned long long int n) {
    if (n == 0) return 64; // Special case for zero
    int count = 0;
    while ((n & 1) == 0) {
        n >>= 1;
        count++;
    }
    return count;
}

void printBits(unsigned long long int n) {
    for (int i = 63; i >= 0; i--) {
        putchar((n & (1ULL << i)) ? '1' : '0');
        if (i % 8 == 0) putchar(' ');
    }
    putchar('\n');
}