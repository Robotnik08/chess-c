#include "common.h"
#include "board.h"
#include "chess.h"
#include "move.h"
#include "magic.h"
#include "FEN.h"

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

int main(int argc, char* argv[]) {
    Board* board = calloc(1, sizeof(Board));

    initMaps();
    initMagic();

    if (argc == 2 && strcmp(argv[1], "engine") == 0) {
        while (1) {
            char input[255];
            scanf("%s", input);
            input[255] = '\0';

            if (strcmp(input, "getmoves") == 0) {
                generateMoves(board);
                printMoves(board, false);
            }

            if (strcmp(input, "getfen") == 0) {
                char* fen = generateFEN(board);
                printf("%s\n", fen);
                free(fen);
            }

            if (strcmp(input, "setfen") == 0) {
                char fen[255];
                scanf("%s", fen);
                parseFEN(fen, board);
            }


            if (strcmp(input, "end") == 0) {
                break;
            }
        }

        freeMagic();
        return 0;
    }

    
    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board);

    while (1) {
        generateMoves(board);
        printMoves(board, true);
        printBoard(board, board->side_to_move);
        movePiece(board, inputMove());
        board->side_to_move = board->side_to_move ? WHITE : BLACK;
    }

    freeMagic();
    return 0;
}