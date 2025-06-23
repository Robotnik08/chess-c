#include "zobrist_hashing.h"
#include "board.h"

extern Board board;

unsigned long long int zobristKeys[BB_MAXVAL][NUM_SQUARES];
unsigned long long int zobristSideKey;
unsigned long long int zobristCastlingKeys[16]; // 2^4 for castling rights (4 bits)
unsigned long long int zobristEnPassantKeys[8]; // 8 files for en passant (1 bit each)

void initZobristHashing(long long int seed) {
    for (int i = 0; i < BB_MAXVAL; i++) {
        for (int j = 0; j < NUM_SQUARES; j++) {
            zobristKeys[i][j] = random64(seed);
        }
    }
    zobristSideKey = random64(seed);
    for (int i = 0; i < 16; i++) {
        zobristCastlingKeys[i] = random64(seed);
    }
    for (int i = 0; i < 8; i++) {
        zobristEnPassantKeys[i] = random64(seed);
    }
}

unsigned long long int getZobristHash() {
    unsigned long long int hash = 0;

    // add piece positions
    for (int i = 0; i < BB_MAXVAL; i++) {
        Bitboard bitboard = board.bitboards[i];
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            hash ^= zobristKeys[i][square];
            bitboard &= bitboard - 1;
        }
    }

    // add side to move
    if (board.side_to_move) {
        hash ^= zobristSideKey;
    }

    // add castling rights
    hash ^= zobristCastlingKeys[board.castling_rights];
    
    // add en passant square
    if (board.en_passant_file >= 0) {
        hash ^= zobristEnPassantKeys[board.en_passant_file];
    }

    return hash;
}