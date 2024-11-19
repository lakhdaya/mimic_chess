#ifndef _DISPLAY_H_
#define _DISPLAY_H_ 1
 
#include <stdbool.h>

#include "pieces.h"


void print_bits(U64 num);

void show_board(U64 board);
void show_boards(U64 board[12]);


#endif