#pragma once
#define PY_SSIZE_T_CLEAN
#include <python3.10/Python.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <time.h>
#include <stdlib.h>


#define U64 unsigned long long 

//information pour occupancy
#define WHITE 0
#define BLACK 1

#define NB_PIECE 6

#define LEFT 2
#define RIGHT 1

//information pour state
#define TURN 0 
#define EP 1 //en passant
#define CWQ 2 //castle:C, queeen side: Q, king side: K, white: W, black: B
#define CWK 3
#define CBQ 4
#define CBK 5

//information pour board
#define p 0
#define n 1
#define b 2
#define r 3
#define q 4
#define k 5
#define P 6
#define N 7
#define B 8
#define R 9
#define Q 10
#define K 11

#define BISHOP b
#define ROOK r
#define KNIGHT n
#define KING k
#define QUEEN q
#define PAWN p

#define BISHOPS_NUMBERS "chess/magic_numbers/bishops"
#define ROOKS_NUMBERS "chess/magic_numbers/rooks"

enum{
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

#define NOT_A_FILE 18374403900871474942ULL

#define NOT_H_FILE 9187201950435737471ULL

#define NOT_HG_FILE 4557430888798830399ULL

#define NOT_AB_FILE 18229723555195321596ULL

#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) (get_bit((bitboard), (square)) ?  (bitboard) ^= (1ULL << (square)):0)


#define encode_move(source, target, piece, promoted, capture, double_p, enpassant, castling) \
    ((source) |         \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double_p << 21) |    \
    (enpassant << 22) | \
    (castling << 23))   \
/* source: chessprogramming.org/Encoding_Moves and chessprogramming on youtube*/

#define get_source(move) (move & 0x3f)

#define get_target(move) ((move & 0xfc0) >> 6)

#define get_piece(move) ((move & 0xf000) >> 12)

#define get_promoted(move) ((move & 0xf0000) >> 16)

#define get_capture(move) (move & 0x100000)

#define get_double(move) (move & 0x200000)

#define get_enpassant(move) (move & 0x400000)

#define get_castling(move) (move & 0x800000)

typedef struct {
    int relevant_bit;
    U64 magic_number;
    U64 attack[512];
    U64 mask;
}Bishop; 



typedef struct {
    int relevant_bit;
    U64 magic_number;
    U64 attack[4096];
    U64 mask;
}Rook; 

typedef struct {
    int moves[256];
    int len;
}Moves;

void init_board(U64 bitboard[12]);


void add_bit(U64 *bits, int position);
U64 add_bit2(U64 bits, int position);

void set_value(U64 *table, int len_table, U64 value);

Moves* malloc_moves(void);
void free_moves(Moves* move);

void print_moves(Moves *moves);
void print_moves_on_board(Moves *moves, int piece);
void print_moves_only_position(Moves *moves);

int get_position_from_letter(char coord[2]);
int get_piece_from_letter(char piece);

U64 rand64(void);
U64 rand64_magic_number(void);

int from_fen_to_bitboard(char [], U64[12]);

void init_bishops(Bishop bishops[64]);
void init_rooks(Rook rooks[64]);

void init_pieces_attacks(int load);

U64 bishop_attack_on_board(U64 bishop, U64 bitboard);
U64 rook_attack_on_board(U64 rook, U64 bitboard);

void reload_occupancy(U64 bitboard[12], U64 occupancy[3]);
void reload_pieces(U64 board[12], U64 pieces[32], int type_pieces[32]);
int reload_attacks(Moves *moves, int state[6], U64 bitboard[12], U64 occupancy[3]);

U64 get_attack(U64 attack[32], U64 pieces[32], U64 piece);

int get_move(Moves* moves, int depart, int arrive, int piece);
void make_move(U64 bitboard[12], U64 occupancy[3], int move, int state[6]);
//sw_board(U64 board);