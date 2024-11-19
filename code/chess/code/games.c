#include "games.h"

int player_vs_player(Moves* moves, U64 board[12]){
    int depart, arrive, piece;
    int move=0;
    while(move == 0){
            if(get_movement(board, &depart, &arrive, &piece)){
                move = get_move(moves, depart, arrive, piece);
            }
        }
    return move;
}

int player_vs_ordi(Moves* moves, U64 board[12], int state[6], int tour_joueur, int *opening_seq, PyObject* pOpeningNotationFunc, PyObject* opening_data){
    int depart, arrive, piece;
    int move = 0;
    if(state[TURN] == tour_joueur){
        while(move == 0){
            if(get_movement(board, &depart, &arrive, &piece)){
                move = get_move(moves, depart, arrive, piece);
            }
        }
    }
    else{
        int ab = state[TURN] ? -1: 1;
        if(*opening_seq){
            Information information = malloc_information(board, state, 0, -ab*1000, 0);
            Game* game_opening;
            game_opening = malloc_game(NULL, information);
            *opening_seq = create_opening(game_opening, pOpeningNotationFunc, opening_data);
            if(opening_seq){
                move = get_best_move(game_opening);
            }
            free_tree(game_opening->son);
            free(game_opening);
        }
        if(!*opening_seq){
            Information information = malloc_information(board, state, 0, -ab*1000, 0);
            Game* game;
            game = malloc_game(NULL, information);
            create_tree_with_alpha_beta(game, 5);
            move = get_best_move(game);
            free_tree(game->son);
            free(game);
        }
    }
    return move;
}

int player_vs_ai(Moves* moves, U64 board[12], int state[6], int tour_joueur, int *opening_seq, PyObject* pOpeningNotationFunc, PyObject* opening_data, PyObject* pFunc, PyObject* modele){
    int depart, arrive, piece;
    int move = 0;
    if(state[TURN] == tour_joueur){
        while(move == 0){
            if(get_movement(board, &depart, &arrive, &piece)){
                move = get_move(moves, depart, arrive, piece);
            }
        }
    }
    else{
        int ab = state[TURN] ? -1: 1;
        if(*opening_seq){
            Information information = malloc_information(board, state, 0, -ab*1000, 0);
            Game* game_opening;
            game_opening = malloc_game(NULL, information);
            *opening_seq = create_opening(game_opening, pOpeningNotationFunc, opening_data);
            if(opening_seq){
                move = get_best_move(game_opening);
                free_tree(game_opening->son);
                free(game_opening);
            }
        }
        if(!*opening_seq){
            Information information = malloc_information(board, state, 0, -ab*1000, 0);
            Game* game;
            game = malloc_game(NULL, information);
            create_tree_with_alpha_beta_with_model(game, 4, pFunc, modele);
            move = get_best_move(game);
            free_tree(game->son);
            free(game);
        }
    }
    return move;
}

int ai_vs_ai(Moves* moves, U64 board[12], int state[6], int tour_ordi, int *opening_seq, PyObject* pOpeningNotationFunc, PyObject* opening_data_ordi, PyObject* opening_data_player, PyObject* pFunc, PyObject* modele){
    if(state[TURN] == tour_ordi){
        return player_vs_ordi(moves, board, state, -1, opening_seq, pOpeningNotationFunc, opening_data_ordi);
    }
    return player_vs_ai(moves, board, state, -1, opening_seq, pOpeningNotationFunc, opening_data_player, pFunc, modele);
}

void load_modele(char* player, PyObject** modele, PyObject** pFunc){
    PyObject* pName, *pModule;
    printf("initialisation modele\n");
    printf("\n");
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    pName = PyUnicode_DecodeFSDefault("mimic.chess_model_to_c");
    pModule = PyImport_Import(pName);
    if(pModule == NULL){
        printf("modul non trouvé\n");
    }
    *modele = load_py(pModule, player);
    *pFunc = PyObject_GetAttrString(pModule, "note_position");
}

void load_opening(char* player, PyObject** opening_data, PyObject **pOpeningNotationFunc){
    PyObject *pNameOpening, *pModule, *pArgsOpening;
    PyObject *pOpeningFunc, *pName;

    char name[120] = "mimic/data/";
    if(strcmp(player, "ordi") == 0){
        pNameOpening = PyUnicode_DecodeFSDefault("mimic/data/complete_opening.csv");

    }
    else{
        strcat(name, player);
        strcat(name, "_opening.csv");
        printf("nom du joueur: %s", name);
        pNameOpening = PyUnicode_DecodeFSDefault(name);

    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    pName = PyUnicode_DecodeFSDefault("mimic.chess_model_to_c");
    pModule = PyImport_Import(pName);

    printf("Creation data des opening\n");
    *pOpeningNotationFunc = PyObject_GetAttrString(pModule, "note_opening");
    pOpeningFunc = PyObject_GetAttrString(pModule, "load_opening");
    pArgsOpening = PyTuple_New(1);
    PyTuple_SetItem(pArgsOpening, 0, pNameOpening);
    *opening_data = PyObject_CallObject(pOpeningFunc, pArgsOpening);

    if(pOpeningFunc == NULL){
        printf("fonction non trouve\n");
    }
    if(*pOpeningNotationFunc == NULL){
        printf("notation fonction non trouve\n");
    }
    else if (PyErr_Occurred()){
        PyErr_Print();
    }
    printf("Fin de la configuration\n");
}

void init_game(char *fen, int choix, char* player){

    int state[6] = {WHITE, 0, 1, 1, 1, 1};
    int nb_turn = 0;
    U64 board[12];
    U64 occupancy[3];
    set_value(board, 12, 0); //initialise les valeurs a 0.
    set_value(occupancy, 3, 0);

    Moves* moves = malloc_moves();
    create_window();
    init_pieces_attacks(1);
    PyObject *modele, *pFunc;
    PyObject *pOpeningNotationFunc;
    PyObject* opening_data_player,*opening_data_ordi;
    from_fen_to_bitboard(fen, board);
    reload_occupancy(board, occupancy);
    Py_Initialize();
    if (choix != 0){
        if(choix == 1){
            if(strcmp(player, "ordi") == 0){
                load_opening(player, &opening_data_ordi, &pOpeningNotationFunc);
            }
            else{
                load_modele(player, &modele, &pFunc);
                load_opening(player, &opening_data_player, &pOpeningNotationFunc);
            }
        }
        else if(choix == 2){
            load_opening("ordi", &opening_data_ordi, &pOpeningNotationFunc);
            load_opening(player, &opening_data_player, &pOpeningNotationFunc);
        }
    }
    int opening_seq = 1;
    Images* images;
    while(reload_attacks(moves, state, board, occupancy) == 1){
        images = draw_board(board);
        int move;
        if(choix == 0){
            move = player_vs_player(moves, board);
        }
        else if(choix == 1){
            if(strcmp(player, "ordi") == 0){
                printf("op: %d\n", opening_seq);
                move = player_vs_ordi(moves, board, state, WHITE, &opening_seq, pOpeningNotationFunc, opening_data_ordi);
            }
            else{
                move = player_vs_ai(moves, board, state, WHITE, &opening_seq, pOpeningNotationFunc, opening_data_player, pFunc, modele);
            }
        }
        else if(choix == 2){
            move = ai_vs_ai(moves, board, state, WHITE, &opening_seq, pOpeningNotationFunc, opening_data_ordi, opening_data_player, pFunc, modele);
        }
        make_move(board, occupancy, move, state);
        free_images(images);
        nb_turn++;
    }
    Py_Finalize();
    printf("fin\n");
    while(1){

    }
}


void create_data(char* player_name){
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    PyObject* pName, *pModule;
    PyObject* pFunc, *pArgs;
    pName = PyUnicode_DecodeFSDefault("mimic.data_creator");
    pModule = PyImport_Import(pName);
    if (pModule == NULL){
        printf("module non trouve\n");
    }
    if (strcmp(player_name, "complete") == 0){
        pFunc = PyObject_GetAttrString(pModule, "extract_games");
        PyObject_CallObject(pFunc, NULL);
    }
    else{
        pFunc = PyObject_GetAttrString(pModule, "extract_games_player");
        if (pModule == NULL){
            printf("fonction non trouve\n");
        }
        pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, PyUnicode_DecodeFSDefault(player_name));
        PyObject_CallObject(pFunc, pArgs);
    }
    if(PyErr_Occurred()){
        PyErr_Print();
    }
}


void create_data_opening(char* player_name){
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    PyObject* pName, *pModule;
    PyObject* pFunc, *pArgs;
    pName = PyUnicode_DecodeFSDefault("mimic.data_creator");
    pModule = PyImport_Import(pName);
    if (pModule == NULL){
        printf("module non trouve\n");
    }
    if (strcmp(player_name, "complete") == 0){
        pFunc = PyObject_GetAttrString(pModule, "extract_games_opening");
        PyObject_CallObject(pFunc, NULL);
    }
    else{
        printf("extraction des données\n");
        pFunc = PyObject_GetAttrString(pModule, "extract_games_opening_player");
        if (pModule == NULL){
            printf("fonction non trouve\n");
        }
        pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, PyUnicode_DecodeFSDefault(player_name));
        PyObject_CallObject(pFunc, pArgs);
    }
    if(PyErr_Occurred()){
        PyErr_Print();
    }
}

void show_names(){
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    PyObject* pName, *pModule;
    PyObject* pFunc;
    pName = PyUnicode_DecodeFSDefault("mimic.data_creator");
    pModule = PyImport_Import(pName);
    pFunc = PyObject_GetAttrString(pModule, "get_player_name");
    PyObject_CallObject(pFunc, NULL); 
    if(PyErr_Occurred()){
        PyErr_Print();
    }
}

void create_model(char* player_name){
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    PyRun_SimpleString("print(sys.path)");
    PyObject* pName, *pModule;
    PyObject* pFunc, *pArgs;
    pName = PyUnicode_DecodeFSDefault("mimic.ia_v3");
    pModule = PyImport_Import(pName);
    if (pModule == NULL){
        printf("module non trouve\n");
    }
    pFunc = PyObject_GetAttrString(pModule, "create_model");
    if (pModule == NULL){
        printf("fonction non trouve\n");
    }
    pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_DecodeFSDefault(player_name));
    PyObject_CallObject(pFunc, pArgs);
    if(PyErr_Occurred()){
        PyErr_Print();
    }
}

void show_names_data(){
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    PyObject* pName, *pModule;
    PyObject* pFunc;
    pName = PyUnicode_DecodeFSDefault("mimic.data_creator");
    pModule = PyImport_Import(pName);
    pFunc = PyObject_GetAttrString(pModule, "get_data_player");
    PyObject_CallObject(pFunc, NULL); 
    if(PyErr_Occurred()){
        PyErr_Print();
    }
}