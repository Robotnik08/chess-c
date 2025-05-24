#include "move.h"


char* getNotation(Move move) {
    int from = FROM(move);
    int to = TO(move);
    int extra = EXTRA(move);

    char* notation = (char*) malloc(10 * sizeof(char));
    int i = 0;
    notation[i++] = 'a' + (from % 8);
    notation[i++] = '1' + (from / 8);
    notation[i++] = 'a' + (to % 8);
    notation[i++] = '1' + (to / 8);

    if (extra == EN_PASSANT) {
        notation[i++] = 'e';
    } else if (extra == PAWN_LEAP) {
        notation[i++] = 'l';
    } else if (extra == CASTLE) {
        notation[i++] = 'o';
    } else if (extra >= PROMOTION_KNIGHT && extra <= PROMOTION_QUEEN) {
        notation[i++] = "nbrq"[extra - PROMOTION_KNIGHT];
    }

    notation[i++] = '\0';

    return notation;
}

void movePiece(Board* board, Move move) {
    int from = FROM(move);
    int to = TO(move);
    int extra = EXTRA(move);

    int valid = 0;

    int piece = getFromLocation(board, from);
    int captured = getFromLocation(board, to);

    // remove the captured piece
    if (captured != EMPTY) {
        board->bitboards[captured] ^= (1LL << to);
    }

    // move the piece
    board->bitboards[piece] ^= (1LL << from);
    
    if (extra >= PROMOTION_KNIGHT && extra <= PROMOTION_QUEEN) {
        piece = extra - PROMOTION_KNIGHT + KNIGHT; // convert promotion type to piece type
    }
    board->bitboards[piece] |= (1LL << to);
}

void printMove(Move move) {
    int from = FROM(move);
    int to = TO(move);
    int extra = EXTRA(move);

    char* notation = getNotation(move);
    printf("%s\n", notation);
    free(notation);
}

void printMoves(Board* board, bool numbered) {
    for (int i = 0; i < board->num_moves; i++) {
        if (numbered) printf("%d. ", i + 1);
        printMove(board->moves[i]);
    }
}