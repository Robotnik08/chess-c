#ifndef CHESS_H
#define CHESS_H

#include "common.h"

#include "board.h"
#include "move.h"

void initMaps();

void initKnightMaps();
void initKingMaps();
void initPawnAttackMaps();
void initDirectionalMasks();

#define UNDER_ATTACK(index) (attackedSquares & (1ULL << index))


int generateMoves(Move* move_list, bool only_captures);

void generateSlidingMoves(int index, int directions);

void generatePawnMoves(int index, byte color);

void generateKnightMoves(int index);

void generateKingMoves(int index);

void filterLegalMoves();

Bitboard getAttackedMap();

void updateBoardState(bool check_insufficient_material);

#endif