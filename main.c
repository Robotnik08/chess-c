#include "common.h"
#include "board.h"
#include "chess.h"
#include "move.h"
#include "magic.h"

Move inputMove() {
    char input[5];
    printf("Enter a move: ");
    scanf("%s", input);
    input[4] = '\0';

    // convert the input to a move
    int from = (input[0] - 'a') + 8 * (input[1] - '1');
    int to = (input[2] - 'a') + 8 * (input[3] - '1');

    return MOVE(from, to, 0);
}

int main() {
    Board* board = calloc(1, sizeof(Board));
    board->bitboards[PAWN | WHITE] = 0x000000000000FF00;
    board->bitboards[PAWN | BLACK] = 0x00FF000000000000;
    board->bitboards[ROOK | WHITE] = 0x0000000000000081;
    board->bitboards[ROOK | BLACK] = 0x8100000000000000;
    board->bitboards[KNIGHT | WHITE] = 0x0000000000000042;
    board->bitboards[KNIGHT | BLACK] = 0x4200000000000000;
    board->bitboards[BISHOP | WHITE] = 0x0000000000000024;
    board->bitboards[BISHOP | BLACK] = 0x2400000000000000;
    board->bitboards[QUEEN | WHITE] = 0x0000000000000008;
    board->bitboards[QUEEN | BLACK] = 0x0800000000000000;
    board->bitboards[KING | WHITE] = 0x0000000000000010;
    board->bitboards[KING | BLACK] = 0x1000000000000000;

    board->side_to_move = WHITE;

    initMaps();
    initMagic();

    while (1) {
        generateMoves(board);
        printMoves(board);
        printBoard(board, board->side_to_move);
        movePiece(board, inputMove());
        board->side_to_move = board->side_to_move ? WHITE : BLACK;
    }

    freeMagic();
    return 0;
}