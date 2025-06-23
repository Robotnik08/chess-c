#include "move.h"
#include "chess.h"

extern Board board;

extern short move_history [1000];
extern byte capture_history [1000];
extern byte castling_rights_history [1000]; 
extern char en_passant_file_history [1000];
extern short halfmove_clock_history [1000];

extern int move_history_count;

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

void makeMove(Move move) {
    int from = FROM(move);
    int to = TO(move);
    int extra = EXTRA(move);

    int piece = getFromLocation(from);
    int captured = getFromLocation(to);

    // remove the captured piece
    if (captured != EMPTY) {
        board.bitboards[captured] ^= (1LL << to);
        move |= IRREVERSABLE; // mark the move as irreversable if a piece was captured
    }

    if (TYPE(piece) == PAWN || extra) {
        move |= IRREVERSABLE;
    }

    int side_to_move = board.side_to_move;

    // update the move history
    capture_history[move_history_count] = captured;
    castling_rights_history[move_history_count] = board.castling_rights;
    en_passant_file_history[move_history_count] = board.en_passant_file;
    halfmove_clock_history[move_history_count] = board.halfmove_clock;

    if (move & IRREVERSABLE) {
        board.halfmove_clock = 0; // reset the halfmove clock if the move is irreversable
    } else {
        board.halfmove_clock++; // increment the halfmove clock if the move is not irreversable
    }

    // move the piece
    board.bitboards[piece] ^= (1LL << from);
    
    if (extra >= PROMOTION_KNIGHT) {
        piece = extra - PROMOTION_KNIGHT + KNIGHT | side_to_move;
    }
    board.bitboards[piece] |= (1LL << to);

    // check if castling is made invalid
    byte castlingrights = board.castling_rights;
    if (castlingrights & CASTLE_WHITE_KINGSIDE && (from == 7 || to == 7)) { // if something captured the rook, or the rook moved
        board.castling_rights ^= CASTLE_WHITE_KINGSIDE;
        move |= IRREVERSABLE; // mark the move as irreversable if castling rights are changed
    } else if (castlingrights & CASTLE_WHITE_QUEENSIDE && (from == 0 || to == 0)) { // if something captured the rook, or the rook moved
        board.castling_rights &= ~CASTLE_WHITE_QUEENSIDE;
        move |= IRREVERSABLE;
    } else if (castlingrights & CASTLE_BLACK_KINGSIDE && (from == 63 || to == 63)) { // if something captured the rook, or the rook moved
        board.castling_rights &= ~CASTLE_BLACK_KINGSIDE;
        move |= IRREVERSABLE;
    } else if (castlingrights & CASTLE_BLACK_QUEENSIDE && (from == 56 || to == 56)) { // if something captured the rook, or the rook moved
        board.castling_rights &= ~CASTLE_BLACK_QUEENSIDE;
        move |= IRREVERSABLE;
    }

    if (from == 4) { // white king moved
        if (extra == CASTLE) {
            if (to == 6) { // kingside castle
                board.bitboards[ROOK | WHITE] ^= (1LL << 7); // remove the rook from the kingside
                board.bitboards[ROOK | WHITE] |= (1LL << 5); // place the rook on the square next to the king
            } else if (to == 2) { // queenside castle
                board.bitboards[ROOK | WHITE] ^= (1LL << 0); // remove the rook from the queenside
                board.bitboards[ROOK | WHITE] |= (1LL << 3); // place the rook on the square next to the king
            }
        }

        board.castling_rights &= ~(CASTLE_WHITE_KINGSIDE | CASTLE_WHITE_QUEENSIDE); // remove white castling rights
    } else if (from == 60) { // black king moved
        if (extra == CASTLE) {
            if (to == 62) { // kingside castle
                board.bitboards[ROOK | BLACK] ^= (1LL << 63); // remove the rook from the kingside
                board.bitboards[ROOK | BLACK] |= (1LL << 61); // place the rook on the square next to the king
            } else if (to == 58) { // queenside castle
                board.bitboards[ROOK | BLACK] ^= (1LL << 56); // remove the rook from the queenside
                board.bitboards[ROOK | BLACK] |= (1LL << 59); // place the rook on the square next to the king
            }
        }

        board.castling_rights &= ~(CASTLE_BLACK_KINGSIDE | CASTLE_BLACK_QUEENSIDE); // remove black castling rights
    }

    if (extra == EN_PASSANT) {
        int ep_square = (side_to_move ? to + 8 : to - 8);
        board.bitboards[PAWN | OTHER_SIDE(side_to_move)] ^= (1LL << ep_square); // remove the pawn that was captured en passant
        board.en_passant_file = -1; // reset en passant square
    } else if (extra == PAWN_LEAP) {
        board.en_passant_file = from % 8; // set the en passant square to the file of the pawn that just moved
    } else {
        board.en_passant_file = -1; // reset en passant square
    }

    board.side_to_move = OTHER_SIDE(side_to_move); // switch sides
    
    move_history[move_history_count] = move;

    move_history_count++;

    if (side_to_move == BLACK) {
        board.fullmove_number++;
    }
}

void unmakeMove() {
    if (move_history_count == 0) {
        return; // no moves to unmake
    }

    board.state = NONE; // reset the board state (unmaking a move always removes a draw or checkmate state)

    int side_to_move = OTHER_SIDE(board.side_to_move);
    board.side_to_move = side_to_move; // switch sides back

    Move move = move_history[--move_history_count];
    int from = FROM(move);
    int to = TO(move);

    board.fullmove_number -= (side_to_move == BLACK); // decrement fullmove number if we just unmade a black move
    board.halfmove_clock = halfmove_clock_history[move_history_count]; // restore the halfmove clock
    board.castling_rights = castling_rights_history[move_history_count];
    board.en_passant_file = en_passant_file_history[move_history_count];
    
    if (!IS_IRREVERSABLE(move)) { // if the move is not irreversable, we can just move the piece back and not have to worry about the rest of the logic
        int piece = getFromLocation(to);
        board.bitboards[piece] ^= (1LL << to);
        board.bitboards[piece] |= (1LL << from);
        return;
    }

    int extra = EXTRA(move);

    int piece = getFromLocation(to);
    int captured = capture_history[move_history_count];

    // restore the captured piece
    if (captured != EMPTY) {
        board.bitboards[captured] |= (1LL << to);
    }

    // move the piece back
    board.bitboards[piece] ^= (1LL << to); // remove the piece from the to square
    if (extra >= PROMOTION_KNIGHT) {
        piece = PAWN | side_to_move; // restore the pawn piece
    }
    board.bitboards[piece] |= (1LL << from);

    if (from == 4) { // white king moved
        if (extra == CASTLE) {
            if (to == 6) { // kingside castle
                board.bitboards[ROOK | WHITE] ^= (1LL << 5); // remove the rook from the square next to the king
                board.bitboards[ROOK | WHITE] |= (1LL << 7); // place the rook back on the kingside
            } else if (to == 2) { // queenside castle
                board.bitboards[ROOK | WHITE] ^= (1LL << 3); // remove the rook from the square next to the king
                board.bitboards[ROOK | WHITE] |= (1LL << 0); // place the rook back on the queenside
            }
        }
    } else if (from == 60) { // black king moved
        if (extra == CASTLE) {
            if (to == 62) { // kingside castle
                board.bitboards[ROOK | BLACK] ^= (1LL << 61); // remove the rook from the square next to the king
                board.bitboards[ROOK | BLACK] |= (1LL << 63); // place the rook back on the kingside
            } else if (to == 58) { // queenside castle
                board.bitboards[ROOK | BLACK] ^= (1LL << 59); // remove the rook from the square next to the king
                board.bitboards[ROOK | BLACK] |= (1LL << 56); // place the rook back on the queenside
            }
        }
    }

    if (extra == EN_PASSANT) {
        int ep_square = (side_to_move ? to + 8 : to - 8);
        board.bitboards[PAWN | OTHER_SIDE(side_to_move)] |= (1LL << ep_square); // restore the pawn that was captured en passant
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

void printMoves(bool numbered, Move* moves, byte num_moves) {
    for (int i = 0; i < num_moves; i++) {
        if (numbered) printf("%d. ", i + 1);
        printMove(moves[i]);
    }
}