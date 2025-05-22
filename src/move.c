#include "move.h"


char* getNotation(Move move) {
    int from = FROM(move);
    int to = TO(move);
    int extra = EXTRA(move);

    char* notation = (char*) malloc(6 * sizeof(char));
    notation[0] = 'a' + (from % 8);
    notation[1] = '1' + (from / 8);
    notation[2] = 'a' + (to % 8);
    notation[3] = '1' + (to / 8);
    notation[4] = '\0';

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