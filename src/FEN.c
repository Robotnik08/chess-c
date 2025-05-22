#include "common.h"
#include "board.h"

void parseFEN(char* fen, Board* board) {
    printf("Parsing FEN: %s\n", fen);
    char* copy = malloc(strlen(fen) + 1);
    strcpy(copy, fen);
    char* token = strtok(copy, " ");
    int index = 0;

    printf("Parsing pieces...\n");
    // pieces
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            char piece = token[index++];
            if (piece >= '1' && piece <= '8') {
                int empty_count = piece - '0';
                for (int i = 0; i < empty_count; i++) {
                    board->bitboards[EMPTY] |= (1ULL << (rank * 8 + file + i));
                }
                file += empty_count - 1;
            } else {
                switch (piece) {
                    case 'P': board->bitboards[PAWN | WHITE] |= (1ULL << (rank * 8 + file)); break;
                    case 'p': board->bitboards[PAWN | BLACK] |= (1ULL << (rank * 8 + file)); break;
                    case 'R': board->bitboards[ROOK | WHITE] |= (1ULL << (rank * 8 + file)); break;
                    case 'r': board->bitboards[ROOK | BLACK] |= (1ULL << (rank * 8 + file)); break;
                    case 'N': board->bitboards[KNIGHT | WHITE] |= (1ULL << (rank * 8 + file)); break;
                    case 'n': board->bitboards[KNIGHT | BLACK] |= (1ULL << (rank * 8 + file)); break;
                    case 'B': board->bitboards[BISHOP | WHITE] |= (1ULL << (rank * 8 + file)); break;
                    case 'b': board->bitboards[BISHOP | BLACK] |= (1ULL << (rank * 8 + file)); break;
                    case 'Q': board->bitboards[QUEEN | WHITE] |= (1ULL << (rank * 8 + file)); break;
                    case 'q': board->bitboards[QUEEN | BLACK] |= (1ULL << (rank * 8 + file)); break;
                    case 'K': board->bitboards[KING | WHITE] |= (1ULL << (rank * 8 + file)); break;
                    case 'k': board->bitboards[KING | BLACK] |= (1ULL << (rank * 8 + file)); break;
                }
            }
        }
        index++;
    }

    printf("Parsing side to move...\n");
    // side to move
    token = strtok(NULL, " ");
    board->side_to_move = (token[0] == 'w') ? WHITE : BLACK;

    printf("Parsing castling rights...\n");
    // castling rights
    token = strtok(NULL, " ");
    board->castling_rights = 0;
    for (int i = 0; token[i] != '\0'; i++) {
        switch (token[i]) {
            case 'K': board->castling_rights |= CASTLE_WHITE_KINGSIDE; break;
            case 'Q': board->castling_rights |= CASTLE_WHITE_QUEENSIDE; break;
            case 'k': board->castling_rights |= CASTLE_BLACK_KINGSIDE; break;
            case 'q': board->castling_rights |= CASTLE_BLACK_QUEENSIDE; break;
        }
    }

    printf("Parsing en passant...\n");
    // en passant
    token = strtok(NULL, " ");
    if (token[0] == '-') {
        board->en_passant_file = -1;
    } else {
        board->en_passant_file = token[0] - 'a';
    }

    printf("Parsing halfmove clock...\n");
    // halfmove clock
    token = strtok(NULL, " ");
    board->halfmove_clock = atoi(token);

    printf("Parsing fullmove number...\n");
    // fullmove number
    token = strtok(NULL, " ");
    board->fullmove_number = atoi(token);

    free(copy);
}

char* generateFEN(Board* board) {
    char* fen = (char*) malloc(100 * sizeof(char));
    int index = 0;

    // pieces
    int empty_count = 0;
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            int piece = EMPTY;
            for (int i = 0; i < BB_MAXVAL; i++) {
                if (board->bitboards[i] & (1ULL << square)) {
                    piece = i;
                    break;
                }
            }
            if (piece == EMPTY) {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    fen[index++] = '0' + empty_count;
                    empty_count = 0;
                }
                fen[index++] = getPieceLetter(piece);
            }
        }
        if (empty_count > 0) {
            fen[index++] = '0' + empty_count;
            empty_count = 0;
        }
        if (rank > 0) {
            fen[index++] = '/';
        }
    }

    fen[index++] = ' ';
    fen[index++] = board->side_to_move == WHITE ? 'w' : 'b';
    fen[index++] = ' ';
    // castling rights
    if (board->castling_rights & CASTLE_WHITE_KINGSIDE) {
        fen[index++] = 'K';
    }
    if (board->castling_rights & CASTLE_WHITE_QUEENSIDE) {
        fen[index++] = 'Q';
    }
    if (board->castling_rights & CASTLE_BLACK_KINGSIDE) {
        fen[index++] = 'k';
    }
    if (board->castling_rights & CASTLE_BLACK_QUEENSIDE) {
        fen[index++] = 'q';
    }
    if (board->castling_rights == 0) {
        fen[index++] = '-';
    }
    fen[index++] = ' ';
    fen[index++] = board->en_passant_file == -1 ? '-' : 'a' + board->en_passant_file;
    if (board->en_passant_file != -1) {
        fen[index++] = board->side_to_move == WHITE ? '6' : '3';
    }
    fen[index++] = ' ';
    // halfmove clock
    int halfmove_clock = board->halfmove_clock;
    if (halfmove_clock < 10) {
        fen[index++] = '0' + halfmove_clock;
    } else {
        fen[index++] = '0' + (halfmove_clock / 10);
        fen[index++] = '0' + (halfmove_clock % 10);
    }

    fen[index++] = ' ';
    // fullmove number
    int fullmove_number = board->fullmove_number;
    if (fullmove_number < 10) {
        fen[index++] = '0' + fullmove_number;
    } else {
        fen[index++] = '0' + (fullmove_number / 10);
        fen[index++] = '0' + (fullmove_number % 10);
    }
    fen[index] = '\0';

    return fen;
}