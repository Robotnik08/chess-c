#include <move.h>

char* getNotation(Move move) {
    int from = FROM(move);
    int to = TO(move);
    int extra = EXTRA(move);

    char* notation = (char*) malloc(6 * sizeof(char));
    notation[0] = 'a' + (from % 8);
    notation[1] = '1' + (from / 8);
    notation[2] = 'a' + (to % 8);
    notation[3] = '1' + (to / 8);
    notation[4] = '\0';

    return notation;
}