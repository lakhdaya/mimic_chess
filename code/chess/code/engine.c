
#include "engine.h"
#include "files.h"
#include "display.h"

extern Bishop bishops[64];
extern Rook rooks[64];
extern U64 pawn_movements[2][64];
extern U64 pawn_attacks[2][64];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];

/* information necessaire pour faire fonctionner la notation de base*/
int PAWN_VALUE = 1;
int KNIGHT_VALUE = 3;
int BISHOP_VALUE = 3;
int ROOK_VALUE = 5;
int QUEEN_VALUE = 9;

float PAWN_TABLE[8][8] = { \
    { 0,  0,  0,  0,  0,  0,  0,  0}, \
    { 5, 10, 10,-20,-20, 10, 10,  5}, \
    { 5, -5,-10,  0,  0,-10, -5,  5}, \
    { 0,  0,  0, 20, 20,  0,  0,  0}, \
    { 5,  5, 10, 25, 25, 10,  5,  5}, \
    {10, 10, 20, 30, 30, 20, 10, 10}, \
    {50, 50, 50, 50, 50, 50, 50, 50}, \
    { 0,  0,  0,  0,  0,  0,  0,  0}  \
};

float KNIGHT_TABLE[8][8] = { \
    {-50, -40, -30, -30, -30, -30, -40, -50}, \
    {-40, -20,   0,   5,   5,   0, -20, -40}, \
    {-30,   5,  10,  15,  15,  10,   5, -30}, \
    {-30,   0,  15,  20,  20,  15,   0, -30}, \
    {-30,   5,  15,  20,  20,  15,   0, -30}, \
    {-30,   0,  10,  15,  15,  10,   0, -30}, \
    {-40, -20,   0,   0,   0,   0, -20, -40}, \
    {-50, -40, -30, -30, -30, -30, -40, -50}  \
};

float BISHOP_TABLE[8][8] = { \
    {-20, -10, -10, -10, -10, -10, -10, -20}, \
    {-10,   5,   0,   0,   0,   0,   5, -10}, \
    {-10,  10,  10,  10,  10,  10,  10, -10}, \
    {-10,   0,  10,  10,  10,  10,   0, -10}, \
    {-10,   5,   5,  10,  10,   5,   5, -10}, \
    {-10,   0,   5,  10,  10,   5,   0, -10}, \
    {-10,   0,   0,   0,   0,   0,   0, -10}, \
    {-20, -10, -10, -10, -10, -10, -10, -20}  \
};

float ROOK_TABLE[8][8] = { \
    { 0,  0,  0,  5,  5,  0,  0,  0}, \
    {-5,  0,  0,  0,  0,  0,  0, -5}, \
    {-5,  0,  0,  0,  0,  0,  0, -5}, \
    {-5,  0,  0,  0,  0,  0,  0, -5}, \
    {-5,  0,  0,  0,  0,  0,  0, -5}, \
    {-5,  0,  0,  0,  0,  0,  0, -5}, \
    { 5, 10, 10, 10, 10, 10, 10,  5}, \
    { 0,  0,  0,  0,  0,  0,  0,  0}  \
};

float QUEEN_TABLE[8][8] = { \
    {-20, -10, -10, -5, -5, -10, -10, -20}, \
    {-10,   0,   0,  0,  0,   0,   0, -10}, \
    {-10,   0,   0,  0,  0,   0,   0, -10}, \
    { -5,   0,   0,  0,  0,   0,   0,  -5}, \
    { -5,   0,   0,  0,  0,   0,   0,  -5}, \
    {-10,   0,   0,  0,  0,   0,   0, -10}, \
    {-10,   0,   0,  0,  0,   0,   0, -10}, \
    {-20, -10, -10, -5, -5, -10, -10, -20}  \
};


Information malloc_information(U64 board[12], int state[6], int depths, int note, int move){
    Information information;
    information.board = board;
    information.state = state;
    information.depths = depths;
    information.note = note;
    information.move = move;
    return information;
}

Game* malloc_game(Game* parent, Information information){
    Game* game = malloc(sizeof(Game));
    game->information = information;
    game->parent = parent;
    game->son = NULL;
    game->brother = NULL;
    game->max = information.state[TURN]?0:1;
    return game;
}

PyObject* load_py(PyObject* pModule, char* player){
    PyObject *pFunc;
    PyObject *pArgs, *pValue;
    char path[50] = "mimic/models/";
    pFunc = PyObject_GetAttrString(pModule, "load_model");
    if (pFunc == NULL){
        printf("undfinied func\n");
        return NULL;
    }
    pArgs = PyTuple_New(1);
    strcat(path, player);
    strcat(path, "_2");
    pValue = PyUnicode_DecodeFSDefault(path);
    PyTuple_SetItem(pArgs, 0, pValue);
    pValue = PyObject_CallObject(pFunc, pArgs);
    if(PyErr_Occurred()){
        PyErr_Print();
    }
    Py_DecRef(pArgs);
    Py_DecRef(pFunc);
    return pValue;
}

void add_brother(Game* game, Game* brother){
    Game* tmp = game;
    while(tmp->brother != NULL){
        tmp = tmp->brother;
    }
    tmp->brother = brother;
}

void maj_min_max(Game* game){
    Game* parent = game->parent;
    Game* son = parent->son;
    while(parent != NULL){
        while(son != NULL){
            if(parent->max){
                if(parent->information.note < son->information.note){
                    parent->information.note = son->information.note;
                }
            }
            else{
                if(parent->information.note > son->information.note){
                    parent->information.note = son->information.note;
                }
            }
            son = son->brother;
        }
        parent = parent->parent;
    }
}

static inline int number_of_bits(U64 bitboard){ // Brian Kernihan's way
    int n_bit = 0;
    while(bitboard){
        n_bit+=1;
        bitboard &= bitboard -1;
    }
    return  n_bit;
} 

static inline void from_U64_to_list(U64 bitboard, bool list_board[64]){
    for(int i=0; i<64; i++){
        list_board[i] = get_bit(bitboard, i);
    }
}

static inline void get_input(U64 board[12], int state[6], int list_boards[LEN_INPUT_MODEL]){
    bool list_board[12][64];
    for(int i=0; i<12; i++){
        from_U64_to_list(board[i], list_board[i]);
    }
    for(int i=0; i<12; i++){
        for(int j=0; j<64; j++){
            list_boards[i*64+j] = list_board[i][j];
        }
    }
    list_boards[64*12] = state[0];
    for(int i=2; i<LEN_INPUT_MODEL-64*12+1; i++){
        list_boards[64*12+i-1] = state[i];
    }
}

static inline float note_position_with_model(U64 board[12], int state[6], PyObject* pFunc, PyObject* model){
    PyObject* pArgs = PyTuple_New(2);       
    PyObject* plist = PyList_New(LEN_INPUT_MODEL);
    PyObject* presult;
    int input_model[LEN_INPUT_MODEL];
    get_input(board, state, input_model);
    for(int i=0; i<LEN_INPUT_MODEL; i++){
        PyList_SetItem(plist, i, PyLong_FromLong(input_model[i]));
    }
    PyTuple_SetItem(pArgs, 0, model);
    PyTuple_SetItem(pArgs, 1, plist);
    presult = PyObject_CallObject(pFunc, pArgs);
    if (PyErr_Occurred()){
        PyErr_Print();
    }
    float note = (float) PyFloat_AsDouble(presult);
    return note;
}

static inline int first_index(U64 bitboard){ // Get first bit at 1 in bits
    if (bitboard){
        return number_of_bits((bitboard & -bitboard) - 1);
    }
    return -1;
}

static inline float note_position(U64 board[12]){
    float sum = 0;
    int x, y;
    //show_boards(board);
    for(int i = 0; i<12; i++){
        int side = i/6? -1:1;
        U64 tmp = board[i];
        while(tmp != 0ULL){
            int position = first_index(tmp);
            x = position%8;
            y = i/6? (7-position/8):position/8;
            switch (i%6){
            case PAWN:
                //printf("valeur pawn %d %d: %f\n", position/8, x, PAWN_TABLE[y][x]*side);
                sum += (PAWN_VALUE + PAWN_TABLE[y][x]/100)*side;
                break;
            case KNIGHT:
                sum += (KNIGHT_VALUE + KNIGHT_TABLE[y][x]/100)*side;
                break;
            case BISHOP:
                sum += (BISHOP_VALUE + BISHOP_TABLE[y][x]/100)*side;
                break;
            case ROOK:
                sum += (ROOK_VALUE + ROOK_TABLE[y][x]/100)*side;
                break;
            case QUEEN:
                sum += (QUEEN_VALUE + QUEEN_TABLE[y][x]/100)*side;
                break;
            case KING:
                break;
            default:
                break;
            }
            pop_bit(tmp, first_index(tmp));
        }
    }
    return sum;
}

static inline float note_opening(U64 board[12], int state[6], PyObject* pFunc, PyObject* opening_data){
    PyObject* pArgs = PyTuple_New(2);
    PyObject* plist = PyList_New(LEN_INPUT_MODEL);
    int input_model[LEN_INPUT_MODEL];
    get_input(board, state, input_model);
    for(int i=0; i<LEN_INPUT_MODEL; i++){
        PyList_SetItem(plist, i, PyLong_FromLong(input_model[i]));
    }
    PyTuple_SetItem(pArgs, 0, opening_data);
    PyTuple_SetItem(pArgs, 1, plist);
    PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
    if (PyErr_Occurred()){
        PyErr_Print();
    }
    return (float) PyFloat_AS_DOUBLE(pResult);
}

static inline void copy_board(U64 src[12], U64 dest[12]){
    for(int i=0; i<12; i++){
        dest[i] = src[i];
    }
}

static inline void copy_state(int src[6], int dest[6]){
    for(int i=0; i<6; i++){
        dest[i] = src[i];
    }
}

static inline void copy_occupancy(U64 src[3], U64 dest[3]){
    for(int i=0; i<3; i++){
        dest[i] = src[i];
    }
}

static inline void sort_moves(Moves* moves){
    int len_capture = 0;
    for(int i=0; i<moves->len; i++){
        if(get_capture(moves->moves[i])){
            int move = moves->moves[len_capture];
            moves->moves[len_capture] = moves->moves[i];
            moves->moves[i] = move;
            len_capture++;
        }
    }
}

inline void create_tree_with_alpha_beta(Game* parent, int max_depths){
    U64 occupancy[3];
    Moves* moves = malloc_moves();
    reload_occupancy(parent->information.board, occupancy);
    reload_attacks(moves, parent->information.state, parent->information.board, occupancy);
    if(parent->information.depths == max_depths){
        //show_boards(parent->information.board);
        if(moves->len == 0){
            parent->information.note = 100*parent->max;
        }
        else{
            parent->information.note = note_position(parent->information.board);
            //show_boards(parent->information.board);
            //printf("note: %f\n", parent->information.note);
        }
        free_moves(moves);
        return;
    }
    sort_moves(moves);
    for(int i=0; i<moves->len; i++){
        U64* next_board = malloc(sizeof(U64)*12);
        int* next_state = malloc(sizeof(int)*6);
        U64 next_occupancy[3];

        copy_board(parent->information.board, next_board);
        copy_state(parent->information.state, next_state);
        copy_occupancy(occupancy, next_occupancy);
        make_move(next_board, next_occupancy, moves->moves[i], next_state);
        int note = parent->max? 100:-100;
        Information information = malloc_information(next_board, next_state, parent->information.depths+1, note, moves->moves[i]);
        Game* next_move = malloc_game(parent, information);
        if(parent->son == NULL){
            parent->son = next_move;
        }
        else{
            add_brother(parent->son, next_move);
        }
        create_tree_with_alpha_beta(next_move, max_depths);
        maj_min_max(next_move);
        if(parent->parent != NULL){
            if(parent->max){
                if(parent->information.note > parent->parent->information.note){
                    break;
                }
            }
            else{
                if(parent->information.note < parent->parent->information.note){
                    break;
                }
            }
        }
    }
    free_moves(moves);
}

inline int create_opening(Game* parent, PyObject* pFunc, PyObject* opening_data){
    U64 occupancy[3];
    Moves* moves = malloc_moves();
    reload_occupancy(parent->information.board, occupancy);
    reload_attacks(moves, parent->information.state, parent->information.board, occupancy);
    int non_trouve=0;
    for(int i=0; i<moves->len; i++){
        U64* next_board = malloc(sizeof(U64)*12);
        int* next_state = malloc(sizeof(int)*6);
        U64 next_occupancy[3];

        copy_board(parent->information.board, next_board);
        copy_state(parent->information.state, next_state);
        copy_occupancy(occupancy, next_occupancy);
        make_move(next_board, next_occupancy, moves->moves[i], next_state);
        float note = note_opening(next_board, next_state, pFunc, opening_data);
        if ((int)note == 2){
            note = parent->information.state[TURN]?1.:0.;
            non_trouve++;
        }
        Information information = malloc_information(next_board, next_state, parent->information.depths+1, note, moves->moves[i]);
        Game* next_move = malloc_game(parent, information);
        next_move->information.note = note;
        if(parent->son == NULL){
            parent->son = next_move;
        }
        else{
            add_brother(parent->son, next_move);
        }
    }
    if(non_trouve == moves->len){
        free_moves(moves);
        return 0;
    }
    free_moves(moves);
    return 1;
}

void save_trees_sons(FILE* file, Game* games, int len){
    Game* tmp = games->son;
    int** positions = malloc(len*sizeof(int));
    int i = 0;
    while(tmp != NULL){
        int* position = malloc(sizeof(int)*64*12+6*sizeof(int));
        get_input(games->information.board, games->information.state, position);
        positions[i] = position;
        tmp = tmp->brother;
        i++;
    }
    save_positions(file, positions, len);
}

void create_tree_with_alpha_beta_with_model(Game* parent, int max_depths, PyObject* pFunc, PyObject* model){
    U64 occupancy[3];
    Moves* moves = malloc_moves();
    reload_occupancy(parent->information.board, occupancy);
    reload_attacks(moves, parent->information.state, parent->information.board, occupancy);
    if(parent->information.depths == max_depths){
        //show_boards(parent->information.board);
        if(moves->len == 0){
            parent->information.note = 100*parent->max;
        }
        else{
            parent->information.note = note_position(parent->information.board);
            parent->information.note+=note_position_with_model(parent->information.board, parent->information.state, pFunc, model);
            //show_boards(parent->information.board);
            //printf("note: %f\n", parent->information.note);
        }
        free_moves(moves);
        return;
    }
    sort_moves(moves);
    for(int i=0; i<moves->len; i++){
        U64* next_board = malloc(sizeof(U64)*12);
        int* next_state = malloc(sizeof(int)*6);
        U64 next_occupancy[3];

        copy_board(parent->information.board, next_board);
        copy_state(parent->information.state, next_state);
        copy_occupancy(occupancy, next_occupancy);
        make_move(next_board, next_occupancy, moves->moves[i], next_state);
        int note = parent->max? 100:-100;
        Information information = malloc_information(next_board, next_state, parent->information.depths+1, note, moves->moves[i]);
        Game* next_move = malloc_game(parent, information);
        if(parent->son == NULL){
            parent->son = next_move;
        }
        else{
            add_brother(parent->son, next_move);
        }
        create_tree_with_alpha_beta(next_move, max_depths);
        maj_min_max(next_move);
        if(parent->parent != NULL){
            if(parent->max){
                if(parent->information.note > parent->parent->information.note){
                    break;
                }
            }
            else{
                if(parent->information.note < parent->parent->information.note){
                    break;
                }
            }
        }
    }
    free_moves(moves);
}

int get_best_move(Game* game){
    Game* tmp = game->son;
    if(tmp == NULL){
        printf("pas de fils\n");
    }
    float max = tmp->information.note;
    int move = tmp->information.move;
    while(tmp != NULL){
        if(game->max == 1){
            if(tmp->information.note > max){
                max = tmp->information.note;
                move = tmp->information.move;
            }
        }
        else{
            if(tmp->information.note < max){
            max = tmp->information.note;
            move = tmp->information.move;
            }
        }      
        tmp = tmp->brother;
    }
    return move;
}

void show_tree(Game* game){
    if(game == NULL){
        return;
    }
    if(game->son == NULL){
        return;
    }
    Game* tmp = game->son;
    while(tmp != NULL){
        printf("note: %f, profondeur: %d, tour: %d %d\n  ", tmp->information.note, tmp->information.depths, tmp->information.state[TURN], tmp->max);
        tmp=tmp->brother;
    }
    tmp = game->son;
    printf("\n----------\n");
    while(tmp != NULL){
        show_tree(tmp->brother);
        tmp=tmp->brother;
    }
}

void show_son(Game* game){
    Game* tmp = game->son;
    while(tmp != NULL){
        show_boards(tmp->information.board);
        printf("note: %f\n", tmp->information.note);
        tmp = tmp->brother;
    }
}

int get_number_of_position(Game* game){
    int nb_current = 0;
    if (game->son == NULL){
        return 1;
    }
    Game* tmp = game->son;
    while(tmp != NULL){
        nb_current+=(get_number_of_position(tmp)+1);
        tmp = tmp->brother;
    }
    return nb_current;
}

void free_tree(Game* game){
    if(game == NULL){
        return;
    }
    
    free_tree(game->brother);  
    free_tree(game->son);
    free(game->information.board);
    free(game->information.state);
    free(game);
}

