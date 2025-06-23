#include "board.h"

#include "common.h"

extern Board board;

bool checkInBounds(Coord coord) {
    return IN_BOUNDS(coord.file, coord.rank);
}

void printBoard(byte display_side) {
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
                if (board.bitboards[i] & (1LL << square)) {
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

byte getFromLocation(byte index) {
    for (int i = PAWN; i <= KING; i++) {
        if (board.bitboards[i | WHITE] & (1LL << index)) {
            return i | WHITE;
        }
        if (board.bitboards[i | BLACK] & (1LL << index)) {
            return i | BLACK;
        }
    }
    return EMPTY;
}

Bitboard getFriendly(byte color) {
    if (color) color = BLACK;
    return board.bitboards[color | PAWN] | board.bitboards[color | KNIGHT] | board.bitboards[color | BISHOP] | board.bitboards[color | ROOK] | board.bitboards[color | QUEEN] | board.bitboards[color | KING];
}

Bitboard getPieceMask() {
    return getFriendly(WHITE) | getFriendly(BLACK);
}

void setSquare(Bitboard *b, byte index) {
    *b |= 1LL << index;
}

void setSquareOnBoard(byte index, byte piece, byte color) {
    for (int i = 0; i < BB_MAXVAL; i++) {
        board.bitboards[i] &= ~(1LL << index);
    }
    board.bitboards[piece + color] |= (1LL << index);
}

int getIndex(byte piece) {
    Bitboard bb = board.bitboards[piece];
    if (bb) {
        return __builtin_ctzll(bb);
    }

    return -1; // piece not found
}