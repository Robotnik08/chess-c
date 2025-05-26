#include "common.h"
#include "board.h"

extern Board board;

void parseFEN(char* fen) {
    // copy FEN
    char* copy = malloc(strlen(fen) + 1);
    strcpy(copy, fen);
    char* ptr = copy;

    // reset board
    for (int i = 0; i < BB_MAXVAL; i++) {
        board.bitboards[i] = 0ULL;
    }
    board.side_to_move = WHITE;
    board.castling_rights = 0;
    board.en_passant_file = -1;
    board.halfmove_clock = 0;
    board.fullmove_number = 1;
    board.state = NONE;

    // parse pieces
    int rank = 7, file = 0;
    while (*ptr && rank >= 0) {
        if (*ptr == '/' || *ptr == ' ') {
            rank--;
            file = 0;
        } else if (*ptr >= '1' && *ptr <= '8') {
            int empty_count = *ptr - '0';
            file += empty_count;
        } else {
            int sq = rank * 8 + file;
            switch (*ptr) {
                case 'P': board.bitboards[PAWN | WHITE] |= (1ULL << sq); break;
                case 'p': board.bitboards[PAWN | BLACK] |= (1ULL << sq); break;
                case 'R': board.bitboards[ROOK | WHITE] |= (1ULL << sq); break;
                case 'r': board.bitboards[ROOK | BLACK] |= (1ULL << sq); break;
                case 'N': board.bitboards[KNIGHT | WHITE] |= (1ULL << sq); break;
                case 'n': board.bitboards[KNIGHT | BLACK] |= (1ULL << sq); break;
                case 'B': board.bitboards[BISHOP | WHITE] |= (1ULL << sq); break;
                case 'b': board.bitboards[BISHOP | BLACK] |= (1ULL << sq); break;
                case 'Q': board.bitboards[QUEEN | WHITE] |= (1ULL << sq); break;
                case 'q': board.bitboards[QUEEN | BLACK] |= (1ULL << sq); break;
                case 'K': board.bitboards[KING | WHITE] |= (1ULL << sq); break;
                case 'k': board.bitboards[KING | BLACK] |= (1ULL << sq); break;
            }
            file++;
        }
        ptr++;
    }

    // skip space
    while (*ptr == ' ') ptr++;

    // side to move
    board.side_to_move = (*ptr == 'w') ? WHITE : BLACK;
    while (*ptr && *ptr != ' ') ptr++;
    while (*ptr == ' ') ptr++;

    // castling rights
    board.castling_rights = 0;
    if (*ptr == '-') {
        ptr++;
    } else {
        while (*ptr && *ptr != ' ') {
            switch (*ptr) {
                case 'K': board.castling_rights |= CASTLE_WHITE_KINGSIDE; break;
                case 'Q': board.castling_rights |= CASTLE_WHITE_QUEENSIDE; break;
                case 'k': board.castling_rights |= CASTLE_BLACK_KINGSIDE; break;
                case 'q': board.castling_rights |= CASTLE_BLACK_QUEENSIDE; break;
            }
            ptr++;
        }
    }
    while (*ptr == ' ') ptr++;

    // en passant
    if (*ptr == '-') {
        board.en_passant_file = -1;
        ptr++;
    } else {
        board.en_passant_file = *ptr - 'a';
        ptr += 2; // skip file and rank
    }
    while (*ptr == ' ') ptr++;

    // halfmove clock
    board.halfmove_clock = atoi(ptr);
    while (*ptr && *ptr != ' ') ptr++;
    while (*ptr == ' ') ptr++;

    // fullmove number
    board.fullmove_number = atoi(ptr);

    free(copy);
}

char* generateFEN() {
    char* fen = (char*) malloc(100 * sizeof(char));
    int index = 0;

    // pieces
    int empty_count = 0;
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            int piece = EMPTY;
            for (int i = 0; i < BB_MAXVAL; i++) {
                if (board.bitboards[i] & (1ULL << square)) {
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
    fen[index++] = board.side_to_move == WHITE ? 'w' : 'b';
    fen[index++] = ' ';
    // castling rights
    if (board.castling_rights & CASTLE_WHITE_KINGSIDE) {
        fen[index++] = 'K';
    }
    if (board.castling_rights & CASTLE_WHITE_QUEENSIDE) {
        fen[index++] = 'Q';
    }
    if (board.castling_rights & CASTLE_BLACK_KINGSIDE) {
        fen[index++] = 'k';
    }
    if (board.castling_rights & CASTLE_BLACK_QUEENSIDE) {
        fen[index++] = 'q';
    }
    if (board.castling_rights == 0) {
        fen[index++] = '-';
    }
    fen[index++] = ' ';
    fen[index++] = board.en_passant_file == -1 ? '-' : 'a' + board.en_passant_file;
    if (board.en_passant_file != -1) {
        fen[index++] = board.side_to_move == WHITE ? '6' : '3';
    }
    fen[index++] = ' ';
    // halfmove clock
    int halfmove_clock = board.halfmove_clock;
    if (halfmove_clock < 10) {
        fen[index++] = '0' + halfmove_clock;
    } else {
        fen[index++] = '0' + (halfmove_clock / 10);
        fen[index++] = '0' + (halfmove_clock % 10);
    }

    fen[index++] = ' ';
    // fullmove number
    int fullmove_number = board.fullmove_number;
    if (fullmove_number < 10) {
        fen[index++] = '0' + fullmove_number;
    } else {
        fen[index++] = '0' + (fullmove_number / 10);
        fen[index++] = '0' + (fullmove_number % 10);
    }
    fen[index] = '\0';

    return fen;
}