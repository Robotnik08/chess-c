#include "chess.h"

#include "board.h"
#include "move.h"
#include "magic.h"


Bitboard knightMaps[64];
Bitboard kingMaps[64];
Bitboard pawnAttackMaps[128];

Bitboard friendlyPieces;
Bitboard enemyPieces;
Bitboard allPieces;
Bitboard allowed_targets;

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
        Bitboard opposite = 0;
        int rank = i / 8;
        int file = i % 8;

        if (rank == 0 || rank == 7) {
            continue;
        }

        if (file < 7) {
            map |= (1LL << ((rank + 1) * 8 + file + 1));
            opposite |= (1LL << ((rank - 1) * 8 + file + 1));
        }
        if (file > 1) {
            map |= (1LL << ((rank + 1) * 8 + file - 1));
            opposite |= (1LL << ((rank - 1) * 8 + file - 1));
        }

        pawnAttackMaps[i] = map;
    }

}

void initMaps() {
    initKnightMaps();
    initKingMaps();
    initPawnAttackMaps();
}

int checkMove(Board* board, Move move) {
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
    board->bitboards[piece] |= (1LL << to);

    return valid;
}

void generateMoves(Board* board) {
    clearMoves(board);
    allPieces = getPieceMask(board);
    friendlyPieces = getFriendly(board, board->side_to_move);
    enemyPieces = getFriendly(board, board->side_to_move ? WHITE : BLACK);
    allowed_targets = ~friendlyPieces;
    for (int i = 0; i < BB_SIZE; i++) {
        byte piece = getFromLocation(board, i);
        byte color = piece & BLACK;

        if (piece == EMPTY || color != board->side_to_move) {
            continue;
        }

        switch (piece & ~BLACK) {
            case BISHOP:
            case ROOK:
            case QUEEN:
                generateSlidingMoves(board, i, piece == QUEEN ? 0b11 : piece == BISHOP ? 0b10 : 0b01);
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

    Bitboard moves = pawnAttackMaps[index] & enemyPieces; // only attack enemy pieces
    for (int i = 0; i < 64; i++) {
        if (moves & (1LL << i)) {
            addMove(board, MOVE(index, i, 0));
        }
    }

    if (getFromLocation(board, index + forward) == EMPTY) {
        addMove(board, MOVE(index, index + forward, 0));

        if (rank == (color == WHITE ? 1 : 6)) {
            if (getFromLocation(board, index + forward * 2) == EMPTY) {
                addMove(board, MOVE(index, index + forward * 2, PAWN_LEAP));
            }
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
}