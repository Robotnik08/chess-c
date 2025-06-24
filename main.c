#include "common.h"
#include "board.h"
#include "chess.h"
#include "move.h"
#include "magic.h"
#include "FEN.h"
#include "zobrist_hashing.h"

#include <time.h>

Board board;

short move_history [1000];
short halfmove_clock_history [1000];
byte capture_history [1000];
byte castling_rights_history [1000];
char en_passant_file_history [1000];

unsigned long long int repetition_history[1000];

int move_history_count;

int perft(int depth, bool root) {
    if (depth == 0) {
        return 1;
    }

    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, false);
    int nodes = 0;

    for (int i = 0; i < num_moves; i++) {
        // Board backup = board; // backup the current board state
        makeMove(moves[i]);
        int count = perft(depth - 1, false);
        nodes += count;
        // if (root) {
        //     char* notation = getNotation(moves[i]);
        //     // printf("%c%c%c%c: %d\n", notation[0], notation[1], notation[2], notation[3], count);
        //     printf("%s: %d\n", notation, count);
        //     free(notation);
        // }
        unmakeMove();
        // board = backup; // restore the board state
    }

    return nodes;
}

void initChess() {
    initZobristHashing(0);
    initMaps();
    initMagic();
}

void cleanupChess() {
    freeMagic();
}

int main(int argc, char* argv[]) {
    initChess();

    if (argc == 2 && strcmp(argv[1], "engine") == 0) {
        while (1) {
            char input[255];
            scanf("%s", input);
            input[254] = '\0';

            if (strcmp(input, "getmoves") == 0) {
                Move moves[MAX_MOVES];
                int n = generateMoves(moves, false);
                printMoves(false, moves, n);
                printf("ok\n");
                fflush(stdout);
            }

            if (strcmp(input, "getfen") == 0) {
                char* fen = generateFEN();
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
                    parseFEN(fen);
                    repetition_history[0] = getZobristHash();
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
                        Move move = stringToMove(token);
                        makeMove(move);
                        repetition_history[move_history_count] = getZobristHash();

                        token = strtok(NULL, " ");
                    }
                    printf("ok\n");
                    fflush(stdout);
                }
            }

            if (strcmp(input, "getstate") == 0) {
                switch (board.state) {
                    case NONE:
                        printf("none\n");
                        break;
                    case CHECKMATE:
                        printf("checkmate\n");
                        break;
                    case STALEMATE:
                        printf("stalemate\n");
                        break;
                    case FIFTY_MOVE_DRAW:
                        printf("fifty_move_draw\n");
                        break;
                    case THREEFOLD_REPETITION:
                        printf("threefold_repetition\n");
                        break;
                    case INSUFFICIENT_MATERIAL:
                        printf("insufficient_material\n");
                        break;
                }
                printf("ok\n");
                fflush(stdout);
            }


            if (strcmp(input, "end") == 0) {
                break;
            }
        }

        cleanupChess();
        return 0;
    }

    
    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    for (int i = 0; i <= 5; i++) {
        clock_t start = clock();
        int count = perft(i, true);
        clock_t end = clock();
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Move count at depth %d: %d (%.3f seconds)\n", i, count, elapsed);
    }

    cleanupChess();
    return 0;
}