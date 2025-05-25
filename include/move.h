#ifndef MOVE_H
#define MOVE_H

#include "common.h"

// format:
// F: from square
// T: to square
// E: extra info
// EEEETTTT TTFFFFFF
typedef short Move;

#include "board.h"

#define MOVE_LOCATION_MASK 0b111111
#define MOVE_TYPE_MASK 0b1111

#define FROM(move) ((move) & MOVE_LOCATION_MASK)
#define TO(move) (((move) >> 6) & MOVE_LOCATION_MASK)
#define EXTRA(move) (((move) >> 12) & MOVE_TYPE_MASK)

#define MOVE(from, to, extra) ((from) | ((to) << 6) | ((extra) << 12))

#define NORMAL 0
#define EN_PASSANT 1
#define PAWN_LEAP 2
#define CASTLE 3
#define PROMOTION_KNIGHT 4
#define PROMOTION_BISHOP 5
#define PROMOTION_ROOK 6
#define PROMOTION_QUEEN 7

// Make sure to free the result of this function
char* getNotation(Move move);

void movePiece(Board* board, Move move);

void printMove(Move move);

void printMoves(Board* board, bool numbered);

#endif