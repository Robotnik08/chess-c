#include "board.h"

#include "common.h"

bool checkInBounds(Coord coord) {
    return IN_BOUNDS(coord.file, coord.rank);
}

void printBoard(Board* board, byte display_side) {
    printf("  |-----------------|  \n");
    for (int rank = 7; rank >= 0; rank--) {
        if (display_side == WHITE) {
            printf("%d | ", rank + 1);
        } else {
            printf("%d | ", 8 - rank);
        }
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (display_side == BLACK) {
                square = 63 - square;
            }

            int piece = -1;
            int piece_color = WHITE;
            for (int i = 0; i < BB_MAXVAL; i++) {
                if (i >= BLACK) {
                    piece_color = BLACK;
                }
                if (board->bitboards[i] & (1LL << square)) {
                    piece = i;
                    break;
                }
            }
            if (piece == -1) {
                printf(".");
            } else {
                printf("%c", getPieceLetter(piece));
            }
            printf(" ");
        }
        printf("|  \n");
    }
    printf("  |-----------------|  \n");
    if (display_side == WHITE) {
        printf("    a b c d e f g h    \n");
    } else {
        printf("    h g f e d c b a    \n");
    }
}

char getPieceLetter(byte piece) {
    return "PNBRQK..pnbrqk.."[piece];
}

byte getFromLocation(Board* board, byte index) {
    for (int i = 0; i < BB_MAXVAL; i++) {
        if (board->bitboards[i] & (1LL << index)) {
            return i;
        }
    }
    return EMPTY;
}

Bitboard getFriendly(Board* board, byte color) {
    if (color) color = BLACK;
    return board->bitboards[color | PAWN] | board->bitboards[color | KNIGHT] | board->bitboards[color | BISHOP] | board->bitboards[color | ROOK] | board->bitboards[color | QUEEN] | board->bitboards[color | KING];
}

Bitboard getPieceMask(Board* board) {
    return getFriendly(board, WHITE) | getFriendly(board, BLACK);
}

void setSquare(Bitboard *b, byte index) {
    *b |= 1LL << index;
}

void setSquareOnBoard(Board* board, byte index, byte piece, byte color) {
    for (int i = 0; i < BB_MAXVAL; i++) {
        board->bitboards[i] &= ~(1LL << index);
    }
    board->bitboards[piece + color] |= (1LL << index);
}

void addMoves(Board* board, short moves[], int len) {
    for (int i = 0; i < len; i++) {
        board->moves[board->num_moves++] = moves[i];
    }
}

void addMove(Board* board, short move) {
    board->moves[board->num_moves++] = move;
}

void clearMoves(Board* board) {
    board->num_moves = 0;
}

int getIndex(Board* board, byte piece) {
    for (int i = 0; i < 64; i++) {
        if (board->bitboards[piece] & (1ULL << i)) {
            return i;
        }
    }
    return -1;
}