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
        piece = extra - PROMOTION_KNIGHT + KNIGHT | board->side_to_move;
    }
    board->bitboards[piece] |= (1LL << to);

    // check if castling is made invalid
    byte castlingrights = board->castling_rights;
    if (castlingrights & CASTLE_WHITE_KINGSIDE && (from == 7 || to == 7)) { // if something captured the rook, or the rook moved
        board->castling_rights ^= CASTLE_WHITE_KINGSIDE;
    } else if (castlingrights & CASTLE_WHITE_QUEENSIDE && (from == 0 || to == 0)) { // if something captured the rook, or the rook moved
        board->castling_rights &= ~CASTLE_WHITE_QUEENSIDE;
    } else if (castlingrights & CASTLE_BLACK_KINGSIDE && (from == 63 || to == 63)) { // if something captured the rook, or the rook moved
        board->castling_rights &= ~CASTLE_BLACK_KINGSIDE;
    } else if (castlingrights & CASTLE_BLACK_QUEENSIDE && (from == 56 || to == 56)) { // if something captured the rook, or the rook moved
        board->castling_rights &= ~CASTLE_BLACK_QUEENSIDE;
    }

    if (from == 4) { // white king moved
        if (extra == CASTLE) {
            if (to == 6) { // kingside castle
                board->bitboards[ROOK | WHITE] ^= (1LL << 7); // remove the rook from the kingside
                board->bitboards[ROOK | WHITE] |= (1LL << 5); // place the rook on the square next to the king
            } else if (to == 2) { // queenside castle
                board->bitboards[ROOK | WHITE] ^= (1LL << 0); // remove the rook from the queenside
                board->bitboards[ROOK | WHITE] |= (1LL << 3); // place the rook on the square next to the king
            }
        }

        board->castling_rights &= ~(CASTLE_WHITE_KINGSIDE | CASTLE_WHITE_QUEENSIDE); // remove white castling rights
    } else if (from == 60) { // black king moved
        if (extra == CASTLE) {
            if (to == 62) { // kingside castle
                board->bitboards[ROOK | BLACK] ^= (1LL << 63); // remove the rook from the kingside
                board->bitboards[ROOK | BLACK] |= (1LL << 61); // place the rook on the square next to the king
            } else if (to == 58) { // queenside castle
                board->bitboards[ROOK | BLACK] ^= (1LL << 56); // remove the rook from the queenside
                board->bitboards[ROOK | BLACK] |= (1LL << 59); // place the rook on the square next to the king
            }
        }

        board->castling_rights &= ~(CASTLE_BLACK_KINGSIDE | CASTLE_BLACK_QUEENSIDE); // remove black castling rights
    }

    if (extra == EN_PASSANT) {
        int ep_square = (board->side_to_move ? to + 8 : to - 8);
        board->bitboards[PAWN | OTHER_SIDE(board->side_to_move)] ^= (1LL << ep_square); // remove the pawn that was captured en passant
        board->en_passant_file = -1; // reset en passant square
    } else if (extra == PAWN_LEAP) {
        board->en_passant_file = from % 8; // set the en passant square to the file of the pawn that just moved
    } else {
        board->en_passant_file = -1; // reset en passant square
    }
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