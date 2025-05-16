#ifndef BOARD_H
#define BOARD_H

#include <common.h>

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

#define CASTLE_WHITE_KINGSIDE 0
#define CASTLE_WHITE_QUEENSIDE 1
#define CASTLE_BLACK_KINGSIDE 2
#define CASTLE_BLACK_QUEENSIDE 4

typedef struct {
    Bitboard bitboards [BB_MAXVAL];

    bool side_to_move;
    byte castling_rights;

    byte en_passant_file;
    short halfmove_clock;


    short moves [MAX_MOVES];
    byte num_moves;
} Board;

typedef short Coord;

#define COORD(x, y) ((x) + (y) << 8)
#define COORD_X(coord) ((coord) & 0xFF)
#define COORD_Y(coord) ((coord) >> 8)

#define IN_BOUNDS(x, y) ((x) >= 0 && (x) < 8 && (y) >= 0 && (y) < 8)

// /**
//  * Creates a new board with the initial position
//  * @return a pointer to the new board (must be freed)
// */
// Board* createBoard();

// /**
//  * Prints the board to stdout
//  * @param board the board to print
// */
// void printBoard(byte display_side);

// char* getPieceIcon(byte piece, byte color);

// char* getPieceLetter(byte piece);

// /**
//  * Gets the piece at a location
//  * @param board the board to get the piece from
//  * @param index the index of the square
//  * @return the piece at the location
// */
// byte getFromLocation(byte index);

// void setSquare(Bitboard *b, byte value);

// void addMoves(short moves[], int len);

// void addMove (short move);

// void clearMoves();


#endif // BOARD_H