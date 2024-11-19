#include "files.h"

void save_rooks_magic_numbers(FILE* file, Rook rooks[64]){
    for(int i=0; i<64; i++){
        fprintf(file, "%lld\n", rooks[i].magic_number);
    }
}

void save_bishops_magic_numbers(FILE* file, Bishop bishops[64]){
    for(int i=0; i<64; i++){
        fprintf(file, "%lld\n", bishops[i].magic_number);
    }
}


void save_positions(FILE* file, int** positions, int len){
    fprintf(file, "positions\n");
    for(int j=0; j<len; j++){
        fprintf(file, "\"[");
        for(int i=0; i<64*12+4; i++){
            fprintf(file, "%d, ", positions[j][i]);
        }
        fprintf(file, "%d", positions[j][64*12+5]);
        fprintf(file, "]\"\n");
    }
}
