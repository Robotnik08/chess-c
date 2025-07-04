#include "chess.h"

#include "board.h"
#include "move.h"
#include "magic.h"
#include "zobrist_hashing.h"

extern Board board;

extern unsigned long long int repetition_history[1000];
extern int move_history_count;

Bitboard knightMaps[64];
Bitboard kingMaps[64];
Bitboard pawnAttackMaps[128];
Bitboard directionalMasks[8][64];

Bitboard friendlyPieces;
Bitboard enemyPieces;
Bitboard allPieces;
Bitboard allowed_targets;
Bitboard pinnedPieces;

Bitboard attackedSquares;

Move* moveList;
int moveCount = 0;

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

int generateMoves(Move* move_list, bool only_captures) {
    moveList = move_list;
    moveCount = 0;

    enPassantAllowed = false;

    int side_to_move = board.side_to_move;

    allPieces = getPieceMask();
    friendlyPieces = getFriendly(side_to_move);
    enemyPieces = getFriendly(OTHER_SIDE(side_to_move));
    allowed_targets = ~friendlyPieces;

    attackedSquares = getAttackedMap();

    // Move generation for each piece
    Bitboard pawnBoard = board.bitboards[PAWN | side_to_move];
    while (pawnBoard) {
        int index = __builtin_ctzll(pawnBoard);
        pawnBoard &= pawnBoard - 1;

        generatePawnMoves(index, side_to_move);
    }

    Bitboard knightBoard = board.bitboards[KNIGHT | side_to_move];
    while (knightBoard) {
        int index = __builtin_ctzll(knightBoard);
        knightBoard &= knightBoard - 1;

        generateKnightMoves(index);
    }

    Bitboard bishopBoard = board.bitboards[BISHOP | side_to_move];
    while (bishopBoard) {
        int index = __builtin_ctzll(bishopBoard);
        bishopBoard &= bishopBoard - 1;

        generateSlidingMoves(index, 0b10); // bishop moves
    }

    Bitboard rookBoard = board.bitboards[ROOK | side_to_move];
    while (rookBoard) {
        int index = __builtin_ctzll(rookBoard);
        rookBoard &= rookBoard - 1;

        generateSlidingMoves(index, 0b01); // rook moves
    }

    Bitboard queenBoard = board.bitboards[QUEEN | side_to_move];
    while (queenBoard) {
        int index = __builtin_ctzll(queenBoard);
        queenBoard &= queenBoard - 1;

        generateSlidingMoves(index, 0b11); // queen moves
    }
    
    generateKingMoves(__builtin_ctzll(board.bitboards[KING | side_to_move]));

    // filter out moves that would leave the king in check
    filterLegalMoves();

    // add hash to repetition history
    updateBoardState(true, false);

    if (board.state != NONE) {
        // the game is over
        moveCount = 0;
    }

    if (only_captures) {
        // filter out non-capturing moves
        int filteredCount = 0;
        for (int i = 0; i < moveCount; i++) {
            if (EXTRA(moveList[i]) == EN_PASSANT || getFromLocation(TO(moveList[i])) != EMPTY) {
                moveList[filteredCount++] = moveList[i];
            }
        }
        moveCount = filteredCount;
    }

    return moveCount;
}

void generateSlidingMoves(int index, int directions) {
    Bitboard moves = 0;

    if (directions & 0b01) {
        moves |= getRookAttacks(index, allPieces);
    }
    if (directions & 0b10) {
        moves |= getBishopAttacks(index, allPieces);
    }

    moves &= allowed_targets;

    while (moves) {
        int targetIndex = __builtin_ctzll(moves);
        moves &= moves - 1;

        moveList[moveCount++] = MOVE(index, targetIndex, 0);
    }
}

void generatePawnMoves(int index, byte color) {
    int rank = index / 8;

    int forward = color == WHITE ? 8 : -8;

    Bitboard attackMap = pawnAttackMaps[index + (color == WHITE ? 0 : 64)];
    Bitboard moves = attackMap & enemyPieces; // only attack enemy pieces
    while (moves) {
        int targetIndex = __builtin_ctzll(moves);
        moves &= moves - 1;

        if (targetIndex < 8 || targetIndex >= 56) {
            // promotion
            moveList[moveCount++] = MOVE(index, targetIndex, PROMOTION_QUEEN);
            moveList[moveCount++] = MOVE(index, targetIndex, PROMOTION_ROOK);
            moveList[moveCount++] = MOVE(index, targetIndex, PROMOTION_BISHOP);
            moveList[moveCount++] = MOVE(index, targetIndex, PROMOTION_KNIGHT);
        } else {
            moveList[moveCount++] = MOVE(index, targetIndex, 0);
        }
    }
    
    // en passant
    byte en_passant_file = board.en_passant_file;
    if (en_passant_file >= 0 && en_passant_file < 8) {
        byte en_passant_index = (color == WHITE ? 5 : 2) * 8 + en_passant_file;
        if (attackMap & (1LL << en_passant_index)) { // can en passant
            moveList[moveCount++] = MOVE(index, en_passant_index, EN_PASSANT);
            enPassantAllowed = true; // set flag so we can easily check later if en passant is allowed
        }
    }

    if (getFromLocation(index + forward) == EMPTY) {
        if (index + forward < 8 || index + forward >= 56) {
            // promotion
            moveList[moveCount++] = MOVE(index, index + forward, PROMOTION_QUEEN);
            moveList[moveCount++] = MOVE(index, index + forward, PROMOTION_ROOK);
            moveList[moveCount++] = MOVE(index, index + forward, PROMOTION_BISHOP);
            moveList[moveCount++] = MOVE(index, index + forward, PROMOTION_KNIGHT);
        } else {
            moveList[moveCount++] = MOVE(index, index + forward, 0);
        }


        if (rank == (color == WHITE ? 1 : 6)) {
            if (getFromLocation(index + forward * 2) == EMPTY) {
                moveList[moveCount++] = MOVE(index, index + forward * 2, PAWN_LEAP);
            }
        }
    }
}

void generateKnightMoves(int index) {
    Bitboard moves = knightMaps[index] & allowed_targets;
    while (moves) {
        int targetIndex = __builtin_ctzll(moves);
        moves &= moves - 1;

        moveList[moveCount++] = MOVE(index, targetIndex, 0);
    }
}

void generateKingMoves(int index) {
    Bitboard moves = kingMaps[index] & allowed_targets;
    while (moves) {
        int targetIndex = __builtin_ctzll(moves);
        moves &= moves - 1;

        moveList[moveCount++] = MOVE(index, targetIndex, 0);
    }

    byte side_to_move = board.side_to_move;
    byte castling_rights = board.castling_rights;


    // castling
    if (castling_rights & (side_to_move ? CASTLE_BLACK_KINGSIDE : CASTLE_WHITE_KINGSIDE)) {
        int rook_index = index + (side_to_move ? 3 : -4);
        // assume the rook is on the kingside and not moved when the flag is set
        // check if the squares between the king and rook are empty
        if (getFromLocation(index + 1) == EMPTY && getFromLocation(index + 2) == EMPTY) {
            moveList[moveCount++] = MOVE(index, index + 2, CASTLE);
        }
    }
    if (castling_rights & (side_to_move ? CASTLE_BLACK_QUEENSIDE : CASTLE_WHITE_QUEENSIDE)) {
        int rook_index = index + (side_to_move ? -4 : 3);
        // assume the rook is on the queenside and not moved when the flag is set
        // check if the squares between the king and rook are empty
        if (getFromLocation(index - 1) == EMPTY && getFromLocation(index - 2) == EMPTY && getFromLocation(index - 3) == EMPTY) {
            moveList[moveCount++] = MOVE(index, index - 2, CASTLE);
        }
    }
}

void filterLegalMoves() {
    int king_index = getIndex(KING | board.side_to_move);
    
    if (king_index < 0) {
        return; // no king found, something is very wrong
    }


    byte color = board.side_to_move;
    byte enemy_color = OTHER_SIDE(board.side_to_move);

    Bitboard all_pieces = getPieceMask();
    Bitboard friendly_pieces = getFriendly(board.side_to_move);

    // sliding pins
    Bitboard enemy_rooks = board.bitboards[ROOK | enemy_color] | board.bitboards[QUEEN | enemy_color];
    Bitboard enemy_bishops = board.bitboards[BISHOP | enemy_color] | board.bitboards[QUEEN | enemy_color];
    Bitboard rook_sliders = getRookAttacks(king_index, enemy_rooks);
    Bitboard bishop_sliders = getBishopAttacks(king_index, enemy_bishops);

    bool solvedEnPassant = !enPassantAllowed;
    int ep_square = -1;
    if (enPassantAllowed) {
        // check if the removal of the en passant pawn is in the sliders maps
        ep_square = (color == WHITE ? 4 : 3) * 8 + board.en_passant_file;
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
            
            int pinnedCount = __builtin_popcountll(pinned_pieces) - 1; // the attacker itself is not pinned

            if (pinnedCount != 1) {

                if (j < 2 && !solvedEnPassant && pinnedCount == 2 && (pinned_pieces & (1LL << ep_square))) {
                    // remove all en passant moves
                    for (int i = 0; i < moveCount; i++) {
                        Move move = moveList[i];
                        if (EXTRA(move) == EN_PASSANT) {
                            moveList[i--] = moveList[--moveCount];
                        }
                    }

                    solvedEnPassant = true;
                }

                if (pinnedCount == 0) {
                    // no pinned pieces, the king is in check
                    // remove all moves that do not end on the mask (unless it's a king move)
                    for (int i = 0; i < moveCount; i++) {
                        Move move = moveList[i];
                        if ((!(pin_mask & (1LL << TO(move))) && FROM(move) != king_index) || (EXTRA(move) == CASTLE)) {
                            // this move is illegal, remove it
                            moveList[i--] = moveList[--moveCount]; // replace with the last move, and check this index again
                        }
                    }
                    
                    // even thought the king is in check, we still need to block pinned pieces
                }

                continue;
            }

            // there is exactly one pinned piece, if this is a friendly piece, it's considered pinned

            Bitboard friendly_pinned = pinned_pieces & friendly_pieces;
            if (friendly_pinned) {
                int pinned_index = __builtin_ctzll(friendly_pinned); // get the index of the pinned piece
                
                // remove all moves that start with the pinned piece and end not on the mask
                for (int i = 0; i < moveCount; i++) {
                    Move move = moveList[i];
                    if (FROM(move) == pinned_index && !(pin_mask & (1LL << TO(move)))) {
                        // this move is illegal, remove it
                        moveList[i--] = moveList[--moveCount]; // replace with the last move, and check this index again
                    }
                }
            }

            if (!solvedEnPassant && (pinned_pieces & (1LL << ep_square))) {
                // remove all en passant moves
                for (int i = 0; i < moveCount; i++) {
                    Move move = moveList[i];
                    if (EXTRA(move) == EN_PASSANT) {
                        moveList[i--] = moveList[--moveCount];
                    }
                }

                solvedEnPassant = true;
            }
        }
    }

    // check if the king is in check by a knight
    Bitboard enemy_knights = board.bitboards[KNIGHT | enemy_color];

    if (knightMaps[king_index] & enemy_knights) {
        int knight_index = __builtin_ctzll(knightMaps[king_index] & enemy_knights);
        // remove all moves that do not end on the knight
        for (int i = 0; i < moveCount; i++) {
            Move move = moveList[i];
            if ((TO(move) != knight_index && FROM(move) != king_index) || (EXTRA(move) == CASTLE)) {
                // this move is illegal, remove it
                moveList[i--] = moveList[--moveCount]; // replace with the last move, and check this index again
            }
        }
    } else {
        // check if the king is in check by a pawn

        Bitboard enemy_pawns = board.bitboards[PAWN | enemy_color];
        Bitboard king_pawn_attacks = pawnAttackMaps[king_index + (color == WHITE ? 0 : 64)];

        if (enemy_pawns & king_pawn_attacks) {
            // the king is in check by a pawn, remove all moves that do not end on the pawn
            int pawn_index = __builtin_ctzll(enemy_pawns & king_pawn_attacks);
            for (int i = 0; i < moveCount; i++) {
                Move move = moveList[i];
                if (((TO(move) != pawn_index && FROM(move) != king_index) || (EXTRA(move) == CASTLE)) && (EXTRA(move) != EN_PASSANT)) {
                    // this move is illegal, remove it
                    moveList[i--] = moveList[--moveCount]; // replace with the last move, and check this index again
                }
            }
        }
    }

    // finally remove king moves that would put the king in check
    for (int i = 0; i < moveCount; i++) {
        Move move = moveList[i];
        if (FROM(move) == king_index) {
            int target_index = TO(move);
            if (UNDER_ATTACK(target_index)) {
                // this move is illegal, remove it
                moveList[i--] = moveList[--moveCount]; // replace with the last move, and check this index again
            }
        }
    }

    if (color == WHITE) {
        // check castling rights
        if (board.castling_rights & CASTLE_WHITE_KINGSIDE) {
            // check if the king can castle kingside
            if (UNDER_ATTACK(5)) {
                // remove the kingside castling move
                for (int i = 0; i < moveCount; i++) {
                    Move move = moveList[i];
                    if (TO(move) == 6 && EXTRA(move) == CASTLE) {
                        moveList[i--] = moveList[--moveCount]; // remove the kingside castling move
                        break;
                    }
                }
            }
        }
        if (board.castling_rights & CASTLE_WHITE_QUEENSIDE) {
            // check if the king can castle queenside
            if (UNDER_ATTACK(3)) {
                // remove the queenside castling move
                for (int i = 0; i < moveCount; i++) {
                    Move move = moveList[i];
                    if (TO(move) == 2 && EXTRA(move) == CASTLE) {
                        moveList[i--] = moveList[--moveCount]; // remove the queenside castling move
                        break;
                    }
                }
            }
        }
    } else {
        // check castling rights
        if (board.castling_rights & CASTLE_BLACK_KINGSIDE) {
            // check if the king can castle kingside
            if (UNDER_ATTACK(61)) {
                // remove the kingside castling move
                for (int i = 0; i < moveCount; i++) {
                    Move move = moveList[i];
                    if (TO(move) == 62 && EXTRA(move) == CASTLE) {
                        moveList[i--] = moveList[--moveCount]; // remove the kingside castling move
                        break;
                    }
                }
            }
        }
        if (board.castling_rights & CASTLE_BLACK_QUEENSIDE) {
            // check if the king can castle queenside
            if (UNDER_ATTACK(59)) {
                // remove the queenside castling move
                for (int i = 0; i < moveCount; i++) {
                    Move move = moveList[i];
                    if (TO(move) == 58 && EXTRA(move) == CASTLE) {
                        moveList[i--] = moveList[--moveCount]; // remove the queenside castling move
                        break;
                    }
                }
            }
        }
    }
}

Bitboard getAttackedMap() {
    Bitboard attacked_map = 0;

    byte other_color = OTHER_SIDE(board.side_to_move);

    Bitboard all_except_king = allPieces & ~(board.bitboards[KING | board.side_to_move]);

    int pawn_offset = other_color == WHITE ? 0 : 64;

    // iterate over all piece types
    Bitboard pawnBoard = board.bitboards[PAWN | other_color];
    while (pawnBoard) {
        int index = __builtin_ctzll(pawnBoard);
        pawnBoard &= pawnBoard - 1;

        attacked_map |= pawnAttackMaps[index + pawn_offset];
    }

    Bitboard knightBoard = board.bitboards[KNIGHT | other_color];
    while (knightBoard) {
        int index = __builtin_ctzll(knightBoard);
        knightBoard &= knightBoard - 1;

        attacked_map |= knightMaps[index];
    }

    Bitboard bishopBoard = board.bitboards[BISHOP | other_color];
    while (bishopBoard) {
        int index = __builtin_ctzll(bishopBoard);
        bishopBoard &= bishopBoard - 1;

        attacked_map |= getBishopAttacks(index, all_except_king);
    }

    Bitboard rookBoard = board.bitboards[ROOK | other_color];
    while (rookBoard) {
        int index = __builtin_ctzll(rookBoard);
        rookBoard &= rookBoard - 1;

        attacked_map |= getRookAttacks(index, all_except_king);
    }

    Bitboard queenBoard = board.bitboards[QUEEN | other_color];
    while (queenBoard) {
        int index = __builtin_ctzll(queenBoard);
        queenBoard &= queenBoard - 1;

        attacked_map |= getBishopAttacks(index, all_except_king) | getRookAttacks(index, all_except_king);
    }

    Bitboard kingBoard = board.bitboards[KING | other_color];
    while (kingBoard) {
        int index = __builtin_ctzll(kingBoard);
        kingBoard &= kingBoard - 1;

        attacked_map |= kingMaps[index];
    }

    return attacked_map;
}

Bitboard getAttackedMapOnlyPawn() {
    Bitboard attacked_map = 0;

    byte other_color = OTHER_SIDE(board.side_to_move);

    Bitboard all_except_king = allPieces & ~(board.bitboards[KING | board.side_to_move]);

    int pawn_offset = other_color == WHITE ? 0 : 64;
    
    // only pawns
    Bitboard pawnBoard = board.bitboards[PAWN | other_color];
    while (pawnBoard) {
        int index = __builtin_ctzll(pawnBoard);
        pawnBoard &= pawnBoard - 1;

        attacked_map |= pawnAttackMaps[index + pawn_offset];
    }

    return attacked_map;
}

void updateBoardState(bool check_insufficient_material, bool only_draws) {
    repetition_history[move_history_count] = getZobristHash();
    if (!only_draws && moveCount == 0) {
        // no moves available, check if the king is in check
        int king_index = getIndex(KING | board.side_to_move);

        if (UNDER_ATTACK(king_index)) {
            board.state = CHECKMATE; // king is in checkmate
        } else {
            board.state = STALEMATE; // king is not in check, but no moves available
        }
        return;
    }

    if (board.halfmove_clock >= 100) {
        board.state = FIFTY_MOVE_DRAW; // fifty move rule
        return;
    }

    // check for threefold repetition
    unsigned long long int hash = repetition_history[move_history_count];
    // check backwards in the history for the same hash
    int count = 1;
    for (int i = move_history_count - 1; i >= 0; i--) {
        if (repetition_history[i] == hash) {
            if (++count >= 3) {
                board.state = THREEFOLD_REPETITION; // threefold repetition
                return;
            }
        }
    }

    if (check_insufficient_material) {

        if ((board.bitboards[PAWN | WHITE] | board.bitboards[PAWN | BLACK]) == 0) { // no pawns on the board
            
            if ((board.bitboards[ROOK | WHITE] | board.bitboards[QUEEN | WHITE]) == 0 &&
                (board.bitboards[ROOK | BLACK] | board.bitboards[QUEEN | BLACK]) == 0) { // no rooks or queens on the board

                int white_knights = __builtin_popcountll(board.bitboards[KNIGHT | WHITE]);
                int white_bishops = __builtin_popcountll(board.bitboards[BISHOP | WHITE]);

                if (white_knights + white_bishops < 2) { // check if white has less than 2 minor pieces
                    int black_knights = __builtin_popcountll(board.bitboards[KNIGHT | BLACK]);
                    int black_bishops = __builtin_popcountll(board.bitboards[BISHOP | BLACK]);

                    if (black_knights + black_bishops < 2) { // check if black has less than 2 minor pieces
                        board.state = INSUFFICIENT_MATERIAL; // insufficient material for both sides
                        return;
                    }
                }
            }
        }
    }

    board.state = NONE; // no special state   
}