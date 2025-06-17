#ifndef ZOBRIST_HASHING_H
#define ZOBRIST_HASHING_H

#include "common.h"
#include "board.h"

void initZobristHashing(long long int seed);

unsigned long long int getZobristHash();

#endif // ZOBRIST_HASHING_H