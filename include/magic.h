#ifndef MAGIC_H
#define MAGIC_H

#include "common.h"

#include "board.h"

#define STRAIGHT 1
#define DIAGONAL 0

Bitboard* createBlockBitboards(Bitboard movementMask, int* returnSize);

Bitboard createSlidingMask(int squareIndex, int ortho);

Bitboard getLegalMovesFromBlockers(int startSquare, Bitboard blockerBitboard, int ortho);

Bitboard* createAttackTable(int square, int ortho, Bitboard magic, int shiftAmount);

Bitboard getRookAttacks(int square, Bitboard blockMap);

Bitboard getBishopAttacks(int square, Bitboard blockMap);

Bitboard getSlidingAttacks(int square, Bitboard blockMap, int ortho);

void initMagic();

void freeMagic();

#endif