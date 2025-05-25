#ifndef FEN_H
#define FEN_H

#include "common.h"
#include "board.h"

void parseFEN(char* fen, Board* board);
char* generateFEN(Board* board);

#endif