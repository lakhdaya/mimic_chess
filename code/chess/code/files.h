#ifndef _FILES_H_
#define _FILES_H_ 1

#include "pieces.h"

void save_rooks_magic_numbers(FILE* file, Rook rooks[64]);
void save_bishops_magic_numbers(FILE* file, Bishop bishops[64]);

void save_positions(FILE* file, int** positions, int len);

#endif