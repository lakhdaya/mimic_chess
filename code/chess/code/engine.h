#include "pieces.h"
#include "files.h"

#define LEN_INPUT_MODEL 773


typedef struct{
    U64* board;
    int* state;
    float note;
    int depths;
    int move;
}Information;

typedef struct node{
    Information information;
    int max;
    struct node* son;
    struct node* parent;
    struct node* brother;
}Game;
extern Bishop bishops[64];
extern Rook rooks[64];
extern U64 pawn_movements[2][64];
extern U64 pawn_attacks[2][64];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];

void create_tree(Game* parent, int depths);
int create_opening(Game* parent, PyObject* pFunc, PyObject* opening_data);
void create_tree_with_alpha_beta(Game* parent, int max_depths);
void create_tree_with_alpha_beta_with_model(Game* parent, int max_depths, PyObject* pFunc, PyObject* model);


Game* malloc_game(Game* parent, Information information);
Information malloc_information(U64 board[12], int state[6], int depths, int note, int move);
void free_tree(Game* game);


void show_tree(Game* game);
void show_son(Game* game);

int get_number_of_position(Game* game);

int get_best_move(Game* game);

PyObject* load_py(PyObject* pyModule, char* player);