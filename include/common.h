#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef unsigned char byte;

int countBits(unsigned long long int n);

int countTrailingZeros(unsigned long long int n);

void printBits(unsigned long long int n);

long long int pseudoRandom(long long int seed);

long long int random64(long long int seed);

#endif