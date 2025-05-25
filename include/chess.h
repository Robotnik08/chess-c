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


void generateMoves(Board* board);

void generateSlidingMoves(Board* board, int index, int directions);

void generatePawnMoves(Board* board, int index, byte color);

void generateKnightMoves(Board* board, int index);

void generateKingMoves(Board* board, int index);

void filterLegalMoves(Board* board);

bool underAttack(Board* board, int index);

void updateBoardState(Board* board);

#endif