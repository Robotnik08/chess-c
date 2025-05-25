#include <stdio.h>
#include <stdlib.h>

unsigned long long int countBits(unsigned long long int n) {
    unsigned long long int count = 0;
    for (; n; count++) n &= (n - 1);
    return count;
}

int main() {
    unsigned long long int n = 0b0000010000010000100001011;
    printf("Number of bits set in %llu: %llu\n", n, countBits(n));
    return 0;
}