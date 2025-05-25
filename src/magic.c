#include "magic.h"

#include "common.h"
#include "board.h"
#include "magic_numbers.h"

Bitboard rookMasks[64];
Bitboard bishopMasks[64];

Bitboard *rookAttacks[64];
Bitboard *bishopAttacks[64];

Coord rook_directions[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
Coord bishop_directions[] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

Bitboard createSlidingMask(int squareIndex, int ortho) {
    Bitboard mask = 0;
    Coord startCoord = {squareIndex % 8, squareIndex / 8};

    for (int i = 0; i < 4; i++) {
        for (int dst = 1; dst < 8; dst++) {
            Coord coord = {
                startCoord.file + (ortho ? rook_directions : bishop_directions)[i].file * dst, 
                startCoord.rank + (ortho ? rook_directions : bishop_directions)[i].rank * dst
            };
            Coord nextCoord = { 
                coord.file + (ortho ? rook_directions : bishop_directions)[i].file, 
                coord.rank + (ortho ? rook_directions : bishop_directions)[i].rank
            };
            
            if (checkInBounds(nextCoord)) {
                setSquare(&mask, coord.rank * 8 + coord.file);
            } else {
                break;
            }
        }
    }
    return mask;
}

Bitboard* createBlockBitboards(Bitboard movementMask, int* returnSize) {
    int moveSquareIndices[NUM_SQUARES];
    int numMoveSquares = 0;
    for (int i = 0; i < NUM_SQUARES; i++) {
        if ((movementMask >> i) & 1) {
            moveSquareIndices[numMoveSquares++] = i;
        }
    }

    int numPatterns = 1 << numMoveSquares; // 2^n
    Bitboard *blockerBitboards = malloc(numPatterns * sizeof(Bitboard));
    if (returnSize) *returnSize = numPatterns;

    for (int patternIndex = 0; patternIndex < numPatterns; patternIndex++) {
        blockerBitboards[patternIndex] = 0;
        for (int bitIndex = 0; bitIndex < numMoveSquares; bitIndex++) {
            int bit = (patternIndex >> bitIndex) & 1;
            blockerBitboards[patternIndex] |= (Bitboard)bit << moveSquareIndices[bitIndex];
        }
    }

    return blockerBitboards;
}

Bitboard getLegalMovesFromBlockers(int startSquare, Bitboard blockerBitboard, int ortho) {
    Bitboard bitboard = 0;
    Coord *directions = ortho ? rook_directions : bishop_directions;
    Coord startCoord = {startSquare % 8, startSquare / 8};

    for (int i = 0; i < 4; i++) {
        for (int dst = 1; dst < 8; dst++) {
            Coord coord = {
                startCoord.file + directions[i].file * dst, 
                startCoord.rank + directions[i].rank * dst
            };

            if (checkInBounds(coord)) {
                int index = coord.rank * 8 + coord.file;
                setSquare(&bitboard, index);
                if (blockerBitboard & (1ULL << index)) {
                    break;
                }
            } else {
                break;
            }
        }
    }

    return bitboard;
}

Bitboard* createAttackTable(int square, int ortho, Bitboard magic, int shiftAmount) {
    int bits = 64 - shiftAmount;
    int size = 1 << bits;
    Bitboard *attacks = malloc(size * sizeof(Bitboard));

    Bitboard mask = ortho ? rookMasks[square] : bishopMasks[square];
    int numBlockers = 0;
    Bitboard* blockerPatterns = createBlockBitboards(mask, &numBlockers);

    for (int i = 0; i < numBlockers; i++) {
        long long int index = (blockerPatterns[i] * magic) >> shiftAmount;
        Bitboard moves = getLegalMovesFromBlockers(square, blockerPatterns[i], ortho);
        attacks[index] = moves;
    }
    
    free(blockerPatterns);

    return attacks;
}

Bitboard getRookAttacks(int square, Bitboard blockMap) {
    long long int key = ((blockMap & rookMasks[square]) * rookMagics[square]) >> rookShifts[square];
    return rookAttacks[square][key];
}

Bitboard getBishopAttacks(int square, Bitboard blockMap) {
    long long int key = ((blockMap & bishopMasks[square]) * bishopMagics[square]) >> bishopShifts[square];
    return bishopAttacks[square][key];
}

Bitboard getSlidingAttacks(int square, Bitboard blockMap, int ortho) {
    return ortho ? getRookAttacks(square, blockMap) : getBishopAttacks(square, blockMap);
}

void initMagic() {
    for (int i = 0; i < 64; i++) {
        rookMasks[i] = createSlidingMask(i, STRAIGHT);
        bishopMasks[i] = createSlidingMask(i, DIAGONAL);
    }

    for (int i = 0; i < 64; i++) {
        rookAttacks[i] = createAttackTable(i, STRAIGHT, rookMagics[i], rookShifts[i]);
        bishopAttacks[i] = createAttackTable(i, DIAGONAL, bishopMagics[i], bishopShifts[i]);
    }

}

void freeMagic() {
    for (int i = 0; i < 64; i++) {
        free(rookAttacks[i]);
        free(bishopAttacks[i]);
    }
}