#ifndef BOARD_H
#define BOARD_H

#include "common.h"

typedef unsigned long long int Bitboard;

#define BB_MAXVAL 14
#define BB_SIZE 64
#define NUM_SQUARES 64
#define MAX_MOVES 256

#define WHITE 0
#define BLACK 8

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

#define PIECE_MASK 0b111 // 3 bits for piece type

#define COLOR(piece) ((piece) & BLACK)
#define TYPE(piece) ((piece) & PIECE_MASK)

#define EMPTY 255

#define CASTLE_WHITE_KINGSIDE 1
#define CASTLE_WHITE_QUEENSIDE 2
#define CASTLE_BLACK_KINGSIDE 4
#define CASTLE_BLACK_QUEENSIDE 8

typedef enum {
    NONE = 0,

    CHECKMATE,
    STALEMATE,
    FIFTY_MOVE_DRAW,
    THREEFOLD_REPETITION,
    INSUFFICIENT_MATERIAL
} BoardState;

typedef struct {
    Bitboard bitboards [BB_MAXVAL];

    bool side_to_move;
    byte castling_rights;

    char en_passant_file;
    short halfmove_clock;
    short fullmove_number;

    BoardState state;
} Board;

typedef struct {
    int file;
    int rank;
} Coord;

#define IN_BOUNDS(x, y) ((x) >= 0 && (x) < 8 && (y) >= 0 && (y) < 8)

#define OTHER_SIDE(side_to_move) ((side_to_move) ? WHITE : BLACK)

bool checkInBounds(Coord coord);

void printBoard(byte display_side);

char getPieceLetter(byte piece);

byte getFromLocation(byte index);

Bitboard getFriendly(byte color);

Bitboard getPieceMask();

void setSquare(Bitboard *b, byte value);

int getIndex(byte piece);

#endif // BOARD_H