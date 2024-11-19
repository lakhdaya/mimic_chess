#ifndef _GAMES_H_
#define _GAMES_H_ 1


#include "graphics.h"

#include "display.h"
#include "engine.h"

extern Bishop bishops[64];
extern Rook rooks[64];
extern U64 pawn_movements[2][64];
extern U64 pawn_attacks[2][64];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];

void init_game(char* fen, int choix, char* player);
void create_data(char* player_name);
void create_data_opening(char* player_name);
void create_model(char* player_name);



void show_names_data();

void show_names();

#endif