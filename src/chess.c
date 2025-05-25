#include "chess.h"

#include "board.h"
#include "move.h"
#include "magic.h"


Bitboard knightMaps[64];
Bitboard kingMaps[64];
Bitboard pawnAttackMaps[128];
Bitboard directionalMasks[8][64];

Bitboard friendlyPieces;
Bitboard enemyPieces;
Bitboard allPieces;
Bitboard allowed_targets;

bool enPassantAllowed = false;

void initKnightMaps() {
    for (int i = 0; i < 64; i++) {
        Bitboard map = 0;
        int rank = i / 8;
        int file = i % 8;

        if (rank + 2 < 8 && file + 1 < 8) {
            map |= (1LL << ((rank + 2) * 8 + file + 1));
        }
        if (rank + 2 < 8 && file - 1 >= 0) {
            map |= (1LL << ((rank + 2) * 8 + file - 1));
        }
        if (rank - 2 >= 0 && file + 1 < 8) {
            map |= (1LL << ((rank - 2) * 8 + file + 1));
        }
        if (rank - 2 >= 0 && file - 1 >= 0) {
            map |= (1LL << ((rank - 2) * 8 + file - 1));
        }
        if (rank + 1 < 8 && file + 2 < 8) {
            map |= (1LL << ((rank + 1) * 8 + file + 2));
        }
        if (rank + 1 < 8 && file - 2 >= 0) {
            map |= (1LL << ((rank + 1) * 8 + file - 2));
        }
        if (rank - 1 >= 0 && file + 2 < 8) {
            map |= (1LL << ((rank - 1) * 8 + file + 2));
        }
        if (rank - 1 >= 0 && file - 2 >= 0) {
            map |= (1LL << ((rank - 1) * 8 + file - 2));
        }

        knightMaps[i] = map;
    }
}

void initKingMaps() {
    for (int i = 0; i < 64; i++) {
        Bitboard map = 0;
        int rank = i / 8;
        int file = i % 8;

        if (rank + 1 < 8) {
            map |= (1LL << ((rank + 1) * 8 + file));
        }
        if (rank - 1 >= 0) {
            map |= (1LL << ((rank - 1) * 8 + file));
        }
        if (file + 1 < 8) {
            map |= (1LL << (rank * 8 + file + 1));
        }
        if (file - 1 >= 0) {
            map |= (1LL << (rank * 8 + file - 1));
        }

        if (rank + 1 < 8 && file + 1 < 8) {
            map |= (1LL << ((rank + 1) * 8 + file + 1));
        }
        if (rank + 1 < 8 && file - 1 >= 0) {
            map |= (1LL << ((rank + 1) * 8 + file - 1));
        }
        if (rank - 1 >= 0 && file + 1 < 8) {
            map |= (1LL << ((rank - 1) * 8 + file + 1));
        }
        if (rank - 1 >= 0 && file - 1 >= 0) {
            map |= (1LL << ((rank - 1) * 8 + file - 1));
        }

        kingMaps[i] = map;
    }
}

void initPawnAttackMaps() {
    for (int i = 0; i < 64; i++) {
        Bitboard map = 0;
        Bitboard opposite_map = 0;
        int rank = i / 8;
        int file = i % 8;

        if (file < 7) {
            map |= (1LL << ((rank + 1) * 8 + file + 1));
            if (rank > 0) {
                opposite_map |= (1LL << ((rank - 1) * 8 + file + 1));
            }
        }
        if (file > 0) {
            if (rank < 7) {
                map |= (1LL << ((rank + 1) * 8 + file - 1));
            }
            opposite_map |= (1LL << ((rank - 1) * 8 + file - 1));
        }

        pawnAttackMaps[i] = map;
        pawnAttackMaps[i + 64] = opposite_map; // store the opposite attacks in the second half of the array
    }

}

// Create directionalmasks for masking sliding pieces for pins
void initDirectionalMasks() {
    for (int i = 0; i < 64; i++) {
        int rank = i / 8;
        int file = i % 8;

        // initialize all directions to 0
        for (int j = 0; j < 8; j++) {
            directionalMasks[j][i] = 0; 
        }

        // horizontal
        for (int j = file + 1; j < 8; j++) {
            directionalMasks[0][i] |= (1LL << (rank * 8 + j)); // right
        }
        for (int j = file - 1; j >= 0; j--) {
            directionalMasks[1][i] |= (1LL << (rank * 8 + j)); // left
        }
        // vertical
        for (int j = rank + 1; j < 8; j++) {
            directionalMasks[2][i] |= (1LL << (j * 8 + file)); // up
        }
        for (int j = rank - 1; j >= 0; j--) {
            directionalMasks[3][i] |= (1LL << (j * 8 + file)); // down
        }
        // diagonal
        for (int j = 1; j < 8; j++) {
            if (rank + j < 8 && file + j < 8) {
                directionalMasks[4][i] |= (1LL << ((rank + j) * 8 + file + j)); // up-right
            }
            if (rank + j < 8 && file - j >= 0) {
                directionalMasks[5][i] |= (1LL << ((rank + j) * 8 + file - j)); // up-left
            }
            if (rank - j >= 0 && file + j < 8) {
                directionalMasks[6][i] |= (1LL << ((rank - j) * 8 + file + j)); // down-right
            }
            if (rank - j >= 0 && file - j >= 0) {
                directionalMasks[7][i] |= (1LL << ((rank - j) * 8 + file - j)); // down-left
            }
        }
    }
}

void initMaps() {
    initKnightMaps();
    initKingMaps();
    initPawnAttackMaps();
    initDirectionalMasks();
}

void generateMoves(Board* board) {
    clearMoves(board);
    enPassantAllowed = false;

    allPieces = getPieceMask(board);
    friendlyPieces = getFriendly(board, board->side_to_move);
    enemyPieces = getFriendly(board, OTHER_SIDE(board->side_to_move));
    allowed_targets = ~friendlyPieces;
    for (int i = 0; i < BB_SIZE; i++) {
        byte piece = getFromLocation(board, i);
        byte color = piece & BLACK;

        if (piece == EMPTY || color != board->side_to_move) {
            continue;
        }

        switch (piece & ~BLACK) {
            case BISHOP:
                generateSlidingMoves(board, i, 0b10);
                break;
            case ROOK:
                generateSlidingMoves(board, i, 0b01);
                break;
            case QUEEN:
                generateSlidingMoves(board, i, 0b11);
                break;
            case PAWN:
                generatePawnMoves(board, i, color);
                break;
            case KNIGHT:
                generateKnightMoves(board, i);
                break;
            case KING:
                generateKingMoves(board, i);
                break;
        }
    }

    filterLegalMoves(board);
}



void generateSlidingMoves(Board* board, int index, int directions) {
    Bitboard moves = 0;

    if (directions & 0b01) {
        moves |= getRookAttacks(index, allPieces);
    }
    if (directions & 0b10) {
        moves |= getBishopAttacks(index, allPieces);
    }

    moves &= allowed_targets;

    for (int i = 0; i < 64; i++) {
        if (moves & (1LL << i)) {
            addMove(board, MOVE(index, i, 0));
        }
    }
}

void generatePawnMoves(Board* board, int index, byte color) {
    int rank = index / 8;

    int forward = color == WHITE ? 8 : -8;

    Bitboard attackMap = pawnAttackMaps[index + (color == WHITE ? 0 : 64)];
    Bitboard moves = attackMap & enemyPieces; // only attack enemy pieces
    for (int i = 0; i < 64; i++) {
        if (moves & (1LL << i)) {
            if (i < 8 || i >= 56) {
                // promotion
                addMove(board, MOVE(index, i, PROMOTION_KNIGHT));
                addMove(board, MOVE(index, i, PROMOTION_BISHOP));
                addMove(board, MOVE(index, i, PROMOTION_ROOK));
                addMove(board, MOVE(index, i, PROMOTION_QUEEN));
            } else {
                addMove(board, MOVE(index, i, 0));
            }
        }
    }

    if (getFromLocation(board, index + forward) == EMPTY) {
        if (index + forward < 8 || index + forward >= 56) {
            // promotion
            addMove(board, MOVE(index, index + forward, PROMOTION_KNIGHT));
            addMove(board, MOVE(index, index + forward, PROMOTION_BISHOP));
            addMove(board, MOVE(index, index + forward, PROMOTION_ROOK));
            addMove(board, MOVE(index, index + forward, PROMOTION_QUEEN));
        } else {
            addMove(board, MOVE(index, index + forward, 0));
        }


        if (rank == (color == WHITE ? 1 : 6)) {
            if (getFromLocation(board, index + forward * 2) == EMPTY) {
                addMove(board, MOVE(index, index + forward * 2, PAWN_LEAP));
            }
        }
    }

    // en passant
    byte en_passant_file = board->en_passant_file;
    if (en_passant_file >= 0 && en_passant_file < 8) {
        byte en_passant_index = (color == WHITE ? 5 : 2) * 8 + en_passant_file;
        if (attackMap & (1LL << en_passant_index)) { // can en passant
            addMove(board, MOVE(index, en_passant_index, EN_PASSANT));
            enPassantAllowed = true; // set flag so we can easily check later if en passant is allowed
        }
    }
}

void generateKnightMoves(Board* board, int index) {
    Bitboard moves = knightMaps[index] & allowed_targets;
    for (int i = 0; i < 64; i++) {
        if (moves & (1LL << i)) {
            addMove(board, MOVE(index, i, 0));
        }
    }
}

void generateKingMoves(Board* board, int index) {
    Bitboard moves = kingMaps[index] & allowed_targets;
    for (int i = 0; i < 64; i++) {
        if (moves & (1LL << i)) {
            addMove(board, MOVE(index, i, 0));
        }
    }

    byte side_to_move = board->side_to_move;
    byte castling_rights = board->castling_rights;


    // castling
    if (castling_rights & (side_to_move ? CASTLE_BLACK_KINGSIDE : CASTLE_WHITE_KINGSIDE)) {
        int rook_index = index + (side_to_move ? 3 : -4);
        // assume the rook is on the kingside and not moved when the flag is set
        // check if the squares between the king and rook are empty
        if (getFromLocation(board, index + 1) == EMPTY && getFromLocation(board, index + 2) == EMPTY) {
            addMove(board, MOVE(index, index + 2, CASTLE));
        }
    }
    if (castling_rights & (side_to_move ? CASTLE_BLACK_QUEENSIDE : CASTLE_WHITE_QUEENSIDE)) {
        int rook_index = index + (side_to_move ? -4 : 3);
        // assume the rook is on the queenside and not moved when the flag is set
        // check if the squares between the king and rook are empty
        if (getFromLocation(board, index - 1) == EMPTY && getFromLocation(board, index - 2) == EMPTY && getFromLocation(board, index - 3) == EMPTY) {
            addMove(board, MOVE(index, index - 2, CASTLE));
        }
    }
}

void filterLegalMoves(Board* board) {
    int king_index = getIndex(board, KING | board->side_to_move);
    
    if (king_index < 0) {
        return; // no king found, something is very wrong
    }


    byte color = board->side_to_move;
    byte enemy_color = OTHER_SIDE(board->side_to_move);

    Bitboard all_pieces = getPieceMask(board);
    Bitboard friendly_pieces = getFriendly(board, board->side_to_move);

    // sliding pins
    Bitboard enemy_rooks = board->bitboards[ROOK | enemy_color] | board->bitboards[QUEEN | enemy_color];
    Bitboard enemy_bishops = board->bitboards[BISHOP | enemy_color] | board->bitboards[QUEEN | enemy_color];
    Bitboard rook_sliders = getRookAttacks(king_index, enemy_rooks);
    Bitboard bishop_sliders = getBishopAttacks(king_index, enemy_bishops);

    bool solvedEnPassant = !enPassantAllowed;
    int ep_square = -1;
    if (enPassantAllowed) {
        // check if the removal of the en passant pawn is in the sliders maps
        ep_square = (color == WHITE ? 4 : 3) * 8 + board->en_passant_file;
        solvedEnPassant = !(((rook_sliders | bishop_sliders) & ~(directionalMasks[2][king_index] | directionalMasks[3][king_index])) & (1LL << ep_square));
    }

    for (int j = 0; j < 8; j++) {
        Bitboard mask = directionalMasks[j][king_index];
        Bitboard pin_mask = 0;
        bool is_xray = false;

        if (j < 4) { // horizontal or vertical
            pin_mask = rook_sliders & mask;
            is_xray = (pin_mask & enemy_rooks) != 0;
        } else { // diagonal
            pin_mask = bishop_sliders & mask;
            is_xray = (pin_mask & enemy_bishops) != 0;
        }

        // only check pins if the king is threatened by a sliding piece, otherwise this side is safe and unpinnable
        if (is_xray) {
            Bitboard pinned_pieces = pin_mask & all_pieces;
            
            int pinnedCount = countBits(pinned_pieces) - 1; // the attacker itself is not pinned

            if (pinnedCount != 1) {

                if (j < 2 && !solvedEnPassant && pinnedCount == 2 && (pinned_pieces & (1LL << ep_square))) {
                    // remove all en passant moves
                    for (int i = 0; i < board->num_moves; i++) {
                        Move move = board->moves[i];
                        if (EXTRA(move) == EN_PASSANT) {
                            board->moves[i--] = board->moves[--board->num_moves];
                        }
                    }

                    solvedEnPassant = true;
                }

                continue; // Either there is no pinned piece, or there are multiple pinned pieces, which are then not locked
            }

            // there is exactly one pinned piece, if this is a friendly piece, it's considered pinned

            Bitboard friendly_pinned = pinned_pieces & friendly_pieces;
            if (friendly_pinned) {
                int pinned_index = countTrailingZeros(friendly_pinned); // get the index of the pinned piece
                
                // remove all moves that start with the pinned piece and end not on the mask
                for (int i = 0; i < board->num_moves; i++) {
                    Move move = board->moves[i];
                    if (FROM(move) == pinned_index && !(mask & (1LL << TO(move)))) {
                        // this move is illegal, remove it
                        board->moves[i--] = board->moves[--board->num_moves]; // replace with the last move, and check this index again
                    }
                }
            }

            if (!solvedEnPassant && (pinned_pieces & (1LL << ep_square))) {
                // remove all en passant moves
                for (int i = 0; i < board->num_moves; i++) {
                    Move move = board->moves[i];
                    if (EXTRA(move) == EN_PASSANT) {
                        board->moves[i--] = board->moves[--board->num_moves];
                    }
                }

                solvedEnPassant = true;
            }
        }
    }
}