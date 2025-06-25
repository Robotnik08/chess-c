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

#define BITBOARD_FULL 0xFFFFFFFFFFFFFFFFULL // Full bitboard (64 bits set to 1)

#define MAX_MOVES 256

#define BB_MAXVAL 14

typedef enum {
    NONE = 0,

    CHECKMATE,
    STALEMATE,
    FIFTY_MOVE_DRAW,
    THREEFOLD_REPETITION,
    INSUFFICIENT_MATERIAL
} BoardState;

typedef struct {
    Bitboard bitboards[BB_MAXVAL];

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

extern int generateMoves(Move* move_list, bool only_captures);

extern Bitboard getAttackedMap();

extern Bitboard getAttackedMapOnlyPawn();

extern void updateBoardState(bool check_insufficient_material);

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

#define NULL_MOVE 0

#define NORMAL 0
#define EN_PASSANT 1
#define PAWN_LEAP 2
#define CASTLE 3
#define PROMOTION_KNIGHT 4
#define PROMOTION_BISHOP 5
#define PROMOTION_ROOK 6
#define PROMOTION_QUEEN 7

#define PROMOTION_MASK 0b100 // 3rd bit is set if the move is a promotion

#define WHITE 0
#define BLACK 8

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

#define EMPTY 255 // special value for empty square

#define PIECE_MASK 0b111 // 3 bits for piece type

#define COLOR(piece) ((piece) & BLACK)
#define TYPE(piece) ((piece) & PIECE_MASK)

#define OTHER_SIDE(side_to_move) ((side_to_move) ? WHITE : BLACK)

extern char* getNotation(Move move);

extern void makeMove(Move move);

extern void unmakeMove();

extern void printMove(Move move);

extern void printMoves(bool numbered, Move* moves, byte num_moves);

extern Move stringToMove(const char* str);

//// zobrist_hashing.h

extern unsigned long long int getZobristHash();


//// main

extern void initChess();

extern void cleanupChess();


#ifdef __cplusplus
}
#endif

#endif // __CHESS_H__