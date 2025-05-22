#ifndef BOARD_H
#define BOARD_H

#include "common.h"

typedef unsigned long long int Bitboard;
typedef unsigned char byte;

#define BB_MAXVAL 14
#define BB_SIZE 64
#define NUM_SQUARES 64
#define MAX_MOVES 218

#define WHITE 0
#define BLACK 8

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

#define EMPTY 255

#define CASTLE_WHITE_KINGSIDE 1
#define CASTLE_WHITE_QUEENSIDE 2
#define CASTLE_BLACK_KINGSIDE 4
#define CASTLE_BLACK_QUEENSIDE 8

typedef struct {
    Bitboard bitboards [BB_MAXVAL];

    bool side_to_move;
    byte castling_rights;

    char en_passant_file;
    short halfmove_clock;
    short fullmove_number;


    short moves [MAX_MOVES];
    byte num_moves;
} Board;

typedef struct {
    int file;
    int rank;
} Coord;

#define IN_BOUNDS(x, y) ((x) >= 0 && (x) < 8 && (y) >= 0 && (y) < 8)

bool checkInBounds(Coord coord);

void printBoard(Board* board, byte display_side);

char getPieceLetter(byte piece);

byte getFromLocation(Board* board, byte index);

Bitboard getFriendly(Board* board, byte color);

Bitboard getPieceMask(Board* board);

void setSquare(Bitboard *b, byte value);

void addMoves(Board* board, short moves[], int len);

void addMove(Board* board, short move);

void clearMoves(Board* board);


#endif // BOARD_H