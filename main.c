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
                printf("ok\n");
                fflush(stdout);
            }

            if (strcmp(input, "getfen") == 0) {
                char* fen = generateFEN(board);
                printf("%s\n", fen);
                printf("ok\n");
                fflush(stdout);
                free(fen);
            }

            if (strcmp(input, "setfen") == 0) {
                char fen[255];
                // Clear input buffer before reading the FEN string
                int c;
                while ((c = getchar()) != '\n' && c != EOF); // discard leftover input
                if (fgets(fen, sizeof(fen), stdin)) {
                    // Remove trailing newline if present
                    size_t len = strlen(fen);
                    if (len > 0 && fen[len - 1] == '\n') {
                        fen[len - 1] = '\0';
                    }
                    parseFEN(fen, board);
                    printf("ok\n");
                    fflush(stdout);
                }
            }

            if (strcmp(input, "setmovehistory") == 0) { // Set the board based from start to move history
                char moves[6 * 1000]; // a Move takes max 6 characters (e.g., "e2e4 " or "e7e8q "), around 16000 moves are possible? let's assume 1000 moves max

                int c;
                while ((c = getchar()) != '\n' && c != EOF); // discard leftover input
                if (fgets(moves, sizeof(moves), stdin)) {
                    // Remove trailing newline if present
                    size_t len = strlen(moves);
                    if (len > 0 && moves[len - 1] == '\n') {
                        moves[len - 1] = '\0';
                    }

                    char* token = strtok(moves, " ");
                    while (token != NULL) {
                        int extra = 0;
                        if (token[4] != '\0') {
                            switch (token[4]) {
                                case 'e': extra = EN_PASSANT; break;
                                case 'l': extra = PAWN_LEAP; break;
                                case 'o': extra = CASTLE; break;
                                case 'n': extra = PROMOTION_KNIGHT; break;
                                case 'b': extra = PROMOTION_BISHOP; break;
                                case 'r': extra = PROMOTION_ROOK; break;
                                case 'q': extra = PROMOTION_QUEEN; break;
                            }
                        }
                        Move move = MOVE((token[0] - 'a') + 8 * (token[1] - '1'),
                                         (token[2] - 'a') + 8 * (token[3] - '1'), extra);
                        movePiece(board, move);
                        // board->side_to_move = board->side_to_move ? WHITE : BLACK;
                        token = strtok(NULL, " ");
                    }
                    printf("ok\n");
                    fflush(stdout);
                }
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