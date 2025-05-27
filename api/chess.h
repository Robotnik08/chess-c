#ifndef __CHESS_H__
#define __CHESS_H__

#ifdef __cplusplus
extern "C" {
#endif

//// common includes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// format:
// F: from square
// T: to square
// E: extra info
// R: irreversable move flag
// REEETTTT TTFFFFFF
typedef short Move;

//// board.h

typedef unsigned char byte;
typedef unsigned long long int Bitboard;

#define MAX_MOVES 256

typedef enum {
    NONE = 0,

    CHECKMATE,
    STALEMATE,
    FIFTY_MOVE_DRAW,
    THREEFOLD_REPETITION,
    INSUFFICIENT_MATERIAL
} BoardState;

typedef struct {
    Bitboard bitboards[14];

    bool side_to_move;
    byte castling_rights;

    char en_passant_file;
    short halfmove_clock;
    short fullmove_number;

    BoardState state;
} Board;

#define OTHER_SIDE(side_to_move) ((side_to_move) ? WHITE : BLACK)

extern void printBoard(byte display_side);

extern byte getFromLocation(byte index);

extern Bitboard getFriendly(byte color);

extern Bitboard getPieceMask();

extern int getIndex(byte piece);


//// chess.h

extern int generateMoves(Move* move_list);


//// FEN.h

extern void parseFEN(char* fen);

extern char* generateFEN();


//// move.h

#define MOVE_LOCATION_MASK 0b111111
#define MOVE_TYPE_MASK 0b111

#define FROM(move) ((move) & MOVE_LOCATION_MASK)
#define TO(move) (((move) >> 6) & MOVE_LOCATION_MASK)
#define EXTRA(move) (((move) >> 12) & MOVE_TYPE_MASK)
#define IS_IRREVERSABLE(move) ((move) >> 15)

#define IRREVERSABLE 0b1000000000000000 // 15th bit is set if the move is irreversable

#define MOVE(from, to, extra) ((from) | ((to) << 6) | ((extra) << 12))

#define NORMAL 0
#define EN_PASSANT 1
#define PAWN_LEAP 2
#define CASTLE 3
#define PROMOTION_KNIGHT 4
#define PROMOTION_BISHOP 5
#define PROMOTION_ROOK 6
#define PROMOTION_QUEEN 7


//// zobrist_hashing.h

extern unsigned long long int getZobristHash();


//// main

extern void init();

extern void cleanup();


#ifdef __cplusplus
}
#endif

#endif // __CHESS_H__