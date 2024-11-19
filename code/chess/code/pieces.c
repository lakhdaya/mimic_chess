#include "pieces.h"

Bishop bishops[64];
Rook rooks[64];
U64 pawn_movements[2][64];
U64 pawn_attacks[2][64];
U64 knight_attacks[64];
U64 king_attacks[64];

char* FROM_LETTER_TO_POSITION[64] = {
"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};

char FROM_LETTER_TO_PIECE[12] = {'p', 'n', 'b', 'r', 'q', 'k', 'P', 'N', 'B', 'R', 'Q', 'K'};

/*l'echiquiers est construit de cette facon:

- Une liste de 12 de longeur contenant les pieces du type N
- Une liste de 3 de longueur contenant les pieces de la couleurs respectives 
et un dernier element contenant toutes les pieces 

*/


const int bishop_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

const int rook_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};



/*sources : chess programming on youtube */


/*fonction basique*/

static inline int number_of_bits(U64 bitboard){ // Brian Kernihan's way
    int n_bit = 0;
    while(bitboard){
        n_bit+=1;
        bitboard &= bitboard -1;
    }
    return  n_bit;
} 

static inline int first_index(U64 bitboard){ // Get first bit at 1 in bits
    if (bitboard){
        return number_of_bits((bitboard & -bitboard) - 1);
    }
    return -1;
}

void set_value(U64 *table, int len_table, U64 value){
    for(int i = 0; i < len_table; i++){
        table[i] = value;
    }
}

void add_bit(U64 *bits, int position){
    U64 bit = 1ULL;
    bit <<= position;
    *bits+=bit;
}

U64 add_bit2(U64 bits, int position){
    U64 bit = 1ULL;
    bit <<= position;
    bits+=bit;
    return bits;
}

int get_position_from_letter(char coord[2]){
    for(int i=0; i<64; i++){
        if (strcmp(FROM_LETTER_TO_POSITION[i], coord) == 0){
            return i;
        }
    }
    return -1;
}

int get_piece_from_letter(char piece){
    for(int i=0; i<12; i++){
        if (FROM_LETTER_TO_PIECE[i] == piece){
            return i;
        }
    }
    return -1;
}

/*allocation des objets*/

Bishop malloc_bishop(int bishop_bit, U64 magic_number, U64 mask, U64 attack[512]){
    Bishop tmp;
    tmp.relevant_bit = bishop_bit;
    tmp.magic_number = magic_number;
    tmp.mask = mask;
    for(int i = 0; i < 512; i++){
        tmp.attack[i] = attack[i];
    }
    tmp.mask = mask;
    return tmp;
}

Rook malloc_rook(int rook_bit, U64 magic_number, U64 mask, U64 attack[4096]){
    Rook tmp;
    tmp.relevant_bit = rook_bit;
    tmp.magic_number = magic_number;
    tmp.mask = mask;
    for(int i = 0; i < 4096; i++){
        tmp.attack[i] = attack[i];
    }
    tmp.mask = mask;
    return tmp;
}

Moves* malloc_moves(void){
    Moves* tmp = (Moves*) malloc(sizeof(Moves));
    tmp->len=0;
    return tmp;
}

void free_moves(Moves* move){
    free(move);
}
/* board function*/

void init_board(U64 bitboard[12]){ // create boards of len of 12
    U64 tmp = 0;
    for(int i = 0; i<12; i++){
        bitboard[i] = tmp;
    }
}


void separate_fen(char fen[], char* separation,  char** separated_fen){
    char *str = strtok(fen, separation);
    int part = 0;
    while(str != NULL){
        separated_fen[part] = str;
        printf("%d\n", part);
        str = strtok(NULL, separation);
        part+=1;
    }
}

int from_fen_to_bitboard(char fen[], U64 bitboard[12]){
    char separation[] = " ";
    char* pieces_position = strtok(fen, separation);
    int i = 0;
    int position = 0;
    printf("%s\n", pieces_position);
    while (position < 64){
        switch (pieces_position[i]){
        case 'p':
            add_bit(&bitboard[PAWN], position);
            break;
        case 'r':
            add_bit(&bitboard[ROOK], position);
            break;
        case 'n':
            add_bit(&bitboard[KNIGHT], position);
            break;
        case 'b':
            add_bit(&bitboard[BISHOP], position);
            break;
        case 'q':
            add_bit(&bitboard[QUEEN], position);
            break;
        case 'k':
            add_bit(&bitboard[KING], position);
            break;
        case 'P':
            add_bit(&bitboard[PAWN+NB_PIECE], position);
            break;
        case 'R':
            add_bit(&bitboard[ROOK+NB_PIECE], position);
            break;
        case 'N':
            add_bit(&bitboard[KNIGHT+NB_PIECE], position);
            break;
        case 'B':
            add_bit(&bitboard[BISHOP+NB_PIECE], position);
            break;
        case 'Q':
            add_bit(&bitboard[QUEEN+NB_PIECE], position);
            break;
        case 'K':
            add_bit(&bitboard[KING+NB_PIECE], position);
            break;
        case '/':
            position-=1;
            break;
        
        default:
            position+=pieces_position[i]-'0'-1;
        }
        position+=1;
        i+=1;
    }
    return 1;
}

/*mouvement basique*/

U64 pawn_movement(U64 pawns, int direction){
    if (direction == 1){
        return pawns >> 8;
    }
    else{
        return pawns << 8;
    }
}

U64 pawn_attack(U64 pawns, int direction){
    U64 attacks = 0ULL;
    if (direction)
    {
        // generate pawn attacks
        if ((pawns >> 7) & NOT_A_FILE) attacks |= (pawns >> 7);
        if ((pawns >> 9) & NOT_H_FILE) attacks |= (pawns >> 9);
    }
    
    // black pawns
    else
    {
        // generate pawn attacks
        if ((pawns << 7) & NOT_H_FILE) attacks |= (pawns << 7);
        if ((pawns << 9) & NOT_A_FILE) attacks |= (pawns << 9);    
    }
    return attacks;
}

U64 knight_attack(U64 knight){
    U64 attacks = 0ULL;

    if (knight & NOT_A_FILE) attacks |= (knight >> 17); // 2B 1D
    if (knight & NOT_H_FILE) attacks |= (knight >> 15); // 2B 1G
    if (knight & NOT_AB_FILE) attacks |= (knight >> 10); // 1B 2D
    if (knight & NOT_HG_FILE) attacks |= (knight >> 6); // 1B 2G

    if (knight & NOT_H_FILE) attacks |= (knight << 17); // 2H 1G
    if (knight & NOT_A_FILE) attacks |= (knight << 15); // 2H 1D
    if (knight & NOT_HG_FILE) attacks |= (knight << 10); // 1B 2G
    if (knight & NOT_AB_FILE) attacks |= (knight << 6);  // 1B 2D

    return attacks;
}

U64 king_attack(U64 king){
    U64 attacks = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;
    
    // set piece on board
    set_bit(bitboard, first_index(king));
    
    // generate king attacks
    if (bitboard >> 8) attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & NOT_H_FILE) attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & NOT_A_FILE) attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & NOT_H_FILE) attacks |= (bitboard >> 1);
    if (bitboard << 8) attacks |= (bitboard << 8);
    if ((bitboard << 9) & NOT_A_FILE) attacks |= (bitboard << 9);
    if ((bitboard << 7) & NOT_H_FILE) attacks |= (bitboard << 7);
    if ((bitboard << 1) & NOT_A_FILE) attacks |= (bitboard << 1);

    return attacks;
}

int loop_condition(int x, int y){
    if((x>=0 )& (x<=7)){
        return ((y>=0) & (y<=7));
    }
    return 0;
}

U64 diagonal_attack(int position, U64 occupancy, int dirx, int diry){
    U64 attacks = 0ULL;
    int x = position%8;
    int y = position/8;
    for(int i = y + diry, j = x + dirx; loop_condition(j, i) ; i+=diry, j+=dirx){ // -1 1
        set_bit(attacks, i*8+j);
        if (get_bit(occupancy, i*8+j) != 0ULL){
            break;
        }
    }
    return attacks;
}

U64 bishop_attack(U64 bishop){
    U64 attacks = 0ULL;
    int position = first_index(bishop);
    int x = position%8;
    int y = position/8;
    for(int i = y + 1, j = x + 1;  i <= 6 && j <= 6; i++, j++){ // H D
        attacks |= (1ULL << (i * 8 + j));
    }
    for(int i = y - 1, j = x + 1;  i >= 1 && j <= 6; i--, j++){ // B D
        attacks |= (1ULL << (i * 8 + j));
    }
    for(int i = y + 1, j = x - 1;  i <= 6 && j >= 1; i++, j--){ // H G
        attacks |= (1ULL << (i * 8 + j));
    }
    for(int i = y - 1, j = x - 1;  i >= 1 && j >= 1; i--, j--){ // B G
        attacks |= (1ULL << (i * 8 + j));
    }
    return attacks;
}

U64 bishop_attack_on_board(U64 bishop, U64 bitboard){
    U64 attacks = 0ULL;
    int position = first_index(bishop);
    attacks|=diagonal_attack(position, bitboard, 1, 1);
    attacks|=diagonal_attack(position, bitboard, 1, -1);
    attacks|=diagonal_attack(position, bitboard, -1, -1);
    attacks|=diagonal_attack(position, bitboard, -1, 1);

    return attacks;
}

U64 rook_attack(U64 rook){
    U64 attacks = 0ULL;
    int position = first_index(rook);
    int x = position%8;
    int y = position/8;

    for (int i = y + 1; i<= 6; i++){
        attacks |= (1ULL << (i * 8 + x));
    }
    for (int i = x + 1; i<= 6; i++){
        attacks |= (1ULL << (y * 8 + i));
    }
    for (int i = y - 1; i>= 1; i--){
        attacks |= (1ULL << (i * 8 + x));
    }
    for (int i = x - 1; i>= 1; i--){
        attacks |= (1ULL << (y * 8 + i));
    }
    return attacks;
}

U64 file_attack(int position, U64 occupancy){
    U64 attacks = 0ULL;
    int x = position%8;
    int y = position/8;
    for (int i = x + 1; i<= 7; i++){
        attacks |= (1ULL << (y * 8 + i));
         if((1ULL << (y * 8 + i)) & occupancy){
            break;
        }
    }
    for (int i = x - 1; i>= 0; i--){
        attacks |= (1ULL << (y * 8 + i));
         if((1ULL << (y * 8 + i)) & occupancy){
            break;
        }
    }
    return attacks;
}

U64 rank_attack(int position, U64 bitboard){
    U64 attacks = 0ULL;
    int x = position%8;
    int y = position/8;
    for (int i = y + 1; i<= 7; i++){
        attacks |= (1ULL << (i * 8 + x));
        if((1ULL << (i * 8 + x)) & bitboard){
            break;
        }
    }
    for (int i = y - 1; i>= 0; i--){
        attacks |= (1ULL << (i * 8 + x));
         if((1ULL << (i * 8 + x)) & bitboard){
            break;
        }
    }
    return attacks;
}

U64 rook_attack_on_board(U64 rook, U64 bitboard){
    U64 attacks = 0ULL;
    int position = first_index(rook);
    attacks|=file_attack(position, bitboard);
    attacks|=rank_attack(position, bitboard);
    return attacks;
}

U64 file_xray_attack(int position, U64 occupancy){
    int x = position%8 + 1;
    int y = position/8;
    U64 blockers = 0ULL;
    while((blockers == 0ULL) & (x<7)){
        blockers = get_bit(occupancy, x+8*y);
        x++;
    }
    U64 tmp = occupancy & ~blockers;
    x = position%8 - 1;
    blockers = 0ULL;
    while((blockers == 0ULL) & (x>0)){
        blockers = get_bit(occupancy, x+8*y);
        x--;
    }
    tmp&=~blockers; 
    return file_attack(position, tmp);  
}

U64 rank_xray_attack(int position, U64 occupancy){
    int x = position%8;
    int y = position/8 + 1;
    U64 blockers = 0ULL;
    while((blockers == 0ULL) & (y<7)){
        blockers = get_bit(occupancy, x+8*y);
        y++;
    }
    U64 tmp = occupancy & ~blockers;
    y = position/8 - 1;
    blockers = 0ULL;
    while((blockers == 0ULL) & (y>0)){
        blockers = get_bit(occupancy, x+8*y);
        y--;
    }
    tmp&=~blockers; 
    return rank_attack(position, tmp);
}

U64 diagonal_xray_attack(int position, U64 occupancy, int dirx, int diry){
    int x = position%8+dirx;
    int y = position/8+diry;
    U64 blockers = 0ULL;
    while((blockers == 0ULL) & loop_condition(x, y)){
        blockers = get_bit(occupancy, x+y*8);
        x+=dirx;
        y+=diry;
    }
    U64 tmp = occupancy & ~blockers;
    return diagonal_attack(position, tmp, dirx, diry);
}   

U64 set_occupancy(int index, int bits_in_attacks, U64 attacks){ // algo
    U64 occupancy = 0ULL;
    for(int i = 0; i < bits_in_attacks; i++){
        int position = first_index(attacks);
        pop_bit(attacks, position);
        if (index & (1 << i)){
            occupancy |= (1ULL << position);
        }
    }
    return occupancy;
}   

/* best magic so far
chessprogramming.org/Best_magics_so_far
*/

U64 rand64(void){
    U64 a = (U64) (rand() & 0xFFFF);
    U64 e = (U64) (rand() & 0xFFFF);
    U64 c = (U64) (rand() & 0xFFFF);
    U64 d = (U64) (rand() & 0xFFFF);
    return a | e << 16 | c << 32 | d << 48;
}


U64 rand64_magic_number(void){
    return rand64() & rand64() & rand64() & rand64();
}

U64 find_magic_number(U64 bitboard, int relevant_bits, int bishop, int number_of_test){
    U64 occupancies[4096];
    U64 attacks[4096];
    U64 used_attacks[4096];

    U64 attack_mask = bishop ? bishop_attack(bitboard): rook_attack(bitboard);
    int occupancy_indicies = 1 << relevant_bits;

    for (int i = 0; i < occupancy_indicies; i++){
        occupancies[i] = set_occupancy(i, relevant_bits, attack_mask);
        attacks[i] = bishop ? bishop_attack_on_board(bitboard, occupancies[i]) : rook_attack_on_board(bitboard, occupancies[i]);
    }
    for(int i = 0; i<number_of_test; i++){
        U64 magic_number = rand64_magic_number();

        if (number_of_bits((magic_number * attack_mask) & 0xFF000000000000) < 6) // FF000...0 huit dernier bit en 1 et le reste en 0.
            continue;
        for(int i = 0; i<4096; i++){
            used_attacks[i] = 0ULL;
        }
        int index, fail;
        for( index = 0, fail = 0; !fail && index < occupancy_indicies; index++){
            int key = (int) ((occupancies[index] * magic_number) >> (64 - relevant_bits));
            if (used_attacks[key] == 0ULL){
                used_attacks[key] = attacks[index];
            }
            else if (used_attacks[key] != attacks[index]){
                fail = 1;
            }
        }
        if (!fail){
            return magic_number;
        }
    }
    printf("Failed to find a magic number.\n");
    return 0ULL;
}
/*
show boards just for test
*/

void sw_board(U64 board){
    for(int i = 0; i<8; i++){
        printf("\n");   
        for (int j = 0; j<8; j++){
            (board & 1ULL << (i*8+j)) ? printf("1") : printf("0");
        }
    }
    printf("\n");
}

void print_moves(Moves *moves){
    for (int i =0; i<moves->len; i++){
        int move = moves->moves[i];
        printf("source: %d\ntarget: %d\npiece: %d\npromoted: %d\ncapture: %d\ndouble: %d/nenpassant: %d\ncastling: %d\n--------------------\n\n", 
        get_source(move),
        get_target(move),
        get_piece(move),
        get_promoted(move),
        get_capture(move),
        get_double(move),
        get_enpassant(move),
        get_castling(move)
        );
    }
}

void print_moves_only_position(Moves *moves){
    for (int i =0; i<moves->len; i++){
        int move = moves->moves[i];
        printf("depart: %d\arrive: %d\n--------------------\n", 
        get_source(move),
        get_target(move));
    }
}

void print_moves_on_board(Moves *moves, int piece){
    U64 board = 0ULL;
    for (int i =0; i<moves->len; i++){
        int move = moves->moves[i];
        if (piece == get_piece(move)){
            int target = get_target(move);
            board = add_bit2(board, target); 
        }
    }
    sw_board(board);
}
/*création de leurs attaques sur le board*/

void load_magic_numbers(FILE* file, U64 magic_numbers[64]){
    U64 magic_number;
    for(int i=0; i<64; i++){
        fscanf(file, "%lld\n", &magic_number);
        magic_numbers[i] = magic_number;
    }
}

void init_bishop_squares(Bishop bishops[64]){
    int square = 0;
    for(U64 i = 1ULL; i != 0ULL; i <<= 1){
        int relevant_bit = bishop_bits[square];
        U64 magic_number =  find_magic_number(i, relevant_bit, 1, 100000000);
        U64 mask = bishop_attack(i);
        int bishop_bit_count = number_of_bits(mask);
        int bishop_occupancy_indices = (1 << bishop_bit_count);
        U64 bishop_attacks[512];
        for (int j = 0; j < bishop_occupancy_indices; j++){
            U64 occupancy = set_occupancy(j, bishop_bit_count, mask);
            int magic_index = (occupancy * magic_number) >> (64 - relevant_bit);
            bishop_attacks[magic_index] = bishop_attack_on_board(i, occupancy);
        }
        bishops[square] = malloc_bishop(relevant_bit, magic_number, mask, bishop_attacks);
        square += 1;
    }
}

void load_bishop_squares(Bishop bishops[64]){
    int square = 0;
    U64 magic_numbers[64];
    FILE* ptrf = fopen(BISHOPS_NUMBERS, "r");
    load_magic_numbers(ptrf, magic_numbers);
    fclose(ptrf);
    for(U64 i = 1ULL; i != 0ULL; i <<= 1){
        int relevant_bit = bishop_bits[square];
        U64 magic_number =  magic_numbers[square];
        U64 mask = bishop_attack(i);
        int bishop_bit_count = number_of_bits(mask);
        int bishop_occupancy_indices = (1 << bishop_bit_count);
        U64 bishop_attacks[512];
        for (int j = 0; j < bishop_occupancy_indices; j++){
            U64 occupancy = set_occupancy(j, bishop_bit_count, mask);
            int magic_index = (occupancy * magic_number) >> (64 - relevant_bit);
            bishop_attacks[magic_index] = bishop_attack_on_board(i, occupancy);
        }
        bishops[square] = malloc_bishop(relevant_bit, magic_number, mask, bishop_attacks);
        square += 1;
    }
}


void init_rook_squares(Rook rooks[64]){
    int square = 0;
    for(U64 i = 1ULL; i != 0ULL; i <<= 1){
        int relevant_bit = rook_bits[square];
        U64 magic_number =  find_magic_number(i, relevant_bit, 0, 100000000);
        U64 mask = rook_attack(i);
        int rook_bit_count = number_of_bits(mask);
        int rook_occupancy_indices = (1 << rook_bit_count);
        U64 rook_attack[4096];
        for (int j = 0; j < rook_occupancy_indices; j++){
            U64 occupancy = set_occupancy(j, rook_bit_count, mask);
            int magic_index = (occupancy * magic_number) >> (64 - relevant_bit);
            rook_attack[magic_index] = rook_attack_on_board(i, occupancy);
        }
        rooks[square] = malloc_rook(relevant_bit, magic_number, mask, rook_attack);
        square += 1;
    }
}

void load_rook_squares(Rook rooks[64]){
    int square = 0;
    U64 magic_numbers[64];
    FILE* ptrf = fopen(ROOKS_NUMBERS, "r");
    load_magic_numbers(ptrf, magic_numbers);
    fclose(ptrf);
    for(U64 i = 1ULL; i != 0ULL; i <<= 1){
        int relevant_bit = rook_bits[square];
        U64 magic_number =  magic_numbers[square];
        U64 mask = rook_attack(i);
        int rook_bit_count = number_of_bits(mask);
        int rook_occupancy_indices = (1 << rook_bit_count);
        U64 rook_attack[4096];
        for (int j = 0; j < rook_occupancy_indices; j++){
            U64 occupancy = set_occupancy(j, rook_bit_count, mask);
            int magic_index = (occupancy * magic_number) >> (64 - relevant_bit);
            rook_attack[magic_index] = rook_attack_on_board(i, occupancy);
        }
        rooks[square] = malloc_rook(relevant_bit, magic_number, mask, rook_attack);
        square += 1;
    }
}

static inline U64 get_bishop_attacks(U64 board, Bishop bishop){
    board &= bishop.mask;
    board *= bishop.magic_number;
    board >>= 64 - bishop.relevant_bit;
    return bishop.attack[board];
}

static inline U64 get_rook_attacks(U64 board, Rook rook){
    board &= rook.mask;
    board *= rook.magic_number;
    board >>= 64 - rook.relevant_bit;
    return rook.attack[board];
}

/*init toutes le sattaques des pieces*/

void init_pieces_attacks(int load){
    int count = 0;
    printf("init sliders pieces\n");
    for (U64 i = 1; i != 0; i <<= 1){
        pawn_attacks[WHITE][count] = pawn_attack(i, WHITE);
        pawn_attacks[BLACK][count] = pawn_attack(i, BLACK);
        pawn_movements[WHITE][count] = pawn_movement(i, WHITE);
        pawn_movements[BLACK][count] = pawn_movement(i, BLACK);
        knight_attacks[count] = knight_attack(i);
        king_attacks[count] = king_attack(i);
        count++;
    }
    if(load == 0){
        printf("init rook pieces\n");
        init_rook_squares(rooks);
        printf("init bishops\n");
        init_bishop_squares(bishops);
        printf("fin init pieces\n");
    }
    else{
        printf("load rook pieces\n");
        load_rook_squares(rooks);
        printf("load bishops\n");
        load_bishop_squares(bishops);
        printf("fin load pieces\n");
    }
}

U64 put_bishop_attacks(U64 occupancy, int position, Bishop bishops[64]){
    Bishop bishop = bishops[position];
    occupancy &= bishop.mask;
    occupancy *= bishop.magic_number;
    occupancy >>= 64 - bishop.relevant_bit;
    return bishop.attack[occupancy];
}

U64 put_rook_attacks(U64 occupancy, int position, Rook rooks[64]){
    Rook rook = rooks[position];
    occupancy &= rook.mask;
    occupancy *= rook.magic_number;
    occupancy >>= 64 - rook.relevant_bit;
    return rook.attack[occupancy];
}

/* reload occupancy according to board*/

void reload_occupancy(U64 bitboard[12], U64 occupancy[3]){
    for(int i=0; i<3; i++){
        occupancy[i] = 0ULL;
    }
    for(int i = 0; i < 12; i++){
        for(U64 square = 1ULL; square != 0; square <<= 1){
            if (square & bitboard[i]){
                occupancy[i/6] |= square;
            }
        }
    }
    occupancy[2] = occupancy[0] | occupancy[1];
}

/* add a move to list of move*/
static inline void add_move(Moves *moves, int move){
    moves->moves[moves->len] = move;

    moves->len++;
}


/*detection of some kind of situation*/
int is_attacked(int position, U64 opponent_attack){
    if(get_bit(opponent_attack, position) != 0ULL){
        return 1;
    }
    return 0;
}


U64 add_restrictions(U64 bitboard, U64* restriction, int len){
    for(int i=0; i<len; i++){
        bitboard&=restriction[i];
    }
    return bitboard;
}

int get_piece_from_position(U64 bitboard[12], int position){
    for (int i=0; i<12; i++){
        if (get_bit(bitboard[i], position )!= 0ULL){
            return i;
        }
    }
    return 13;
}

U64 check_restriction(U64 attack, U64* restrictions, U64 opponent_attack, int nb_king_attacked, int king_position){
    /* put restriction if player is checked*/
    if (is_attacked(king_position, opponent_attack)){
        for(int i=0; i<nb_king_attacked; i++){
            attack&=restrictions[i];
        }
    }
    return attack;
}

U64 pinned_restriction(U64 attack, int position, U64* x_ray_attacks, int nb_xray_king_attacked){
    for(int i=0; i<nb_xray_king_attacked; i++){
        if (get_bit(x_ray_attacks[i], position) != 0ULL){
            attack&=x_ray_attacks[i];
        }
    }
    return attack;
}

/* pieces generation*/
int add_attack(U64* total_attack, U64 attack[12], U64 xray_attack[12], int nb_attack, int nb_xray_attack, int position, int king_position){
    *total_attack |= attack[nb_attack];
    if(get_bit(attack[nb_attack], king_position) != 0ULL) {
        attack[nb_attack] = add_bit2(attack[nb_attack], position);
        *total_attack |= xray_attack[nb_xray_attack];
        return nb_attack+1;
    }
    return nb_attack;
}

void add_attack_and_xray(U64* total_attack, U64 attack[12], U64* xray_attack, int *nb_attack, int* nb_xray_attack, int position, int king_position){
    *nb_attack = add_attack(total_attack, attack, xray_attack, *nb_attack, *nb_xray_attack, position, king_position);
    U64 tmp = 0ULL;
    *nb_xray_attack = add_attack(&tmp, xray_attack, xray_attack,  *nb_xray_attack, *nb_xray_attack, position, king_position);
}

U64 opponent_bishop_attack(U64 attack[12], U64 xray_attack[12], int *nb_attack, int* nb_xray_attack, U64 occupancy, int position, int king_position){
    U64 total_attack = 0ULL;
    int dir[4][2] = {{1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
    for(int i=0; i<4; i++){
        attack[*nb_attack] = diagonal_attack(position, occupancy, dir[i][0], dir[i][1]);
        xray_attack[*nb_xray_attack] = diagonal_xray_attack(position, occupancy, dir[i][0], dir[i][1]);
        add_attack_and_xray(&total_attack, attack, xray_attack, nb_attack, nb_xray_attack, position, king_position);
    }
    return total_attack;
}

U64 opponent_rook_attack(U64 attack[12], U64 xray_attack[12], int *nb_attack, int* nb_xray_attack, U64 occupancy, int position, int king_position){
    U64 total_attack = 0ULL;
    attack[*nb_attack] = file_attack(position, occupancy);
    xray_attack[*nb_xray_attack] = file_xray_attack(position, occupancy);
    add_attack_and_xray(&total_attack, attack, xray_attack, nb_attack, nb_xray_attack, position, king_position);

    attack[*nb_attack] = rank_attack (position, occupancy);
    xray_attack[*nb_xray_attack] = rank_xray_attack(position, occupancy);
    add_attack_and_xray(&total_attack, attack, xray_attack, nb_attack, nb_xray_attack, position, king_position);
    return total_attack;
}
/* move generator, here we create all moves with restrictions */

int reload_attacks(Moves *moves,int state[6], U64 bitboard[12], U64 occupancy[3]){
    moves->len = 0;
    U64 piece, attack;
    U64 opponent_attack[32];
    U64 opponent_xray_attack[32];
    U64 opponent_attacks = 0ULL;
    int nb_king_attacked = 0;
    int nb_xray_king_attacked = 0;
    int turn = state[TURN];
    int king_position = first_index(bitboard[KING+turn*NB_PIECE]);
    /* get attacks of the piece that doesnt play to check is there is check or pinned pieces, we only calcul attack and xray attack but not movement*/
    for(int i = (1-turn)*NB_PIECE; i<6 + NB_PIECE*(1-turn); i++){
        piece = bitboard[i];
        attack = 0ULL;
        while (piece != 0ULL){
            int position = first_index(piece);
            switch (i%6)
            {
            case PAWN:
                opponent_attacks |= pawn_attacks[i/6][position];
                opponent_attack[nb_king_attacked] = (pawn_attacks[i/6][position] & bitboard[KING+turn*NB_PIECE]);
                if(opponent_attack[nb_king_attacked] != 0ULL){
                    set_bit(opponent_attack[nb_king_attacked], position);
                    nb_king_attacked++;
                }
                break;
            case KNIGHT:
                opponent_attacks |= knight_attacks[position];
                opponent_attack[nb_king_attacked] = (knight_attacks[position] & bitboard[KING+turn*NB_PIECE]);
                if(opponent_attack[nb_king_attacked] != 0ULL){
                    set_bit(opponent_attack[nb_king_attacked], position);
                    nb_king_attacked++;
                }
                break;
            case BISHOP:
                opponent_attacks |= opponent_bishop_attack(opponent_attack, opponent_xray_attack, &nb_king_attacked, &nb_xray_king_attacked, occupancy[2], position, king_position);
                break;
            case ROOK:
                opponent_attacks |= opponent_rook_attack(opponent_attack, opponent_xray_attack, &nb_king_attacked, &nb_xray_king_attacked, occupancy[2], position, king_position);
                break;
            case QUEEN:
                opponent_attacks |= opponent_rook_attack(opponent_attack, opponent_xray_attack, &nb_king_attacked, &nb_xray_king_attacked, occupancy[2], position, king_position);
                opponent_attacks |= opponent_bishop_attack(opponent_attack, opponent_xray_attack, &nb_king_attacked, &nb_xray_king_attacked, occupancy[2], position, king_position);
                break;
            case KING:
                opponent_attacks |= king_attacks[position];
                break;
            default:
                break;
            }
            pop_bit(piece, position);
        }
    }
    //sw_board(opponent_attacks);
    for(int i = turn*NB_PIECE; i<NB_PIECE*(turn+1); i++){
        piece = bitboard[i];
        attack = 0ULL;
        while (piece != 0ULL){
            int position = first_index(piece);
            switch (i%6)
            {
            case PAWN:
                /*regular movement*/
                attack = (pawn_movements[turn][position] & ~occupancy[2]);
                attack = check_restriction(attack, opponent_attack, opponent_attacks, nb_king_attacked, king_position);
                attack = pinned_restriction(attack, position, opponent_xray_attack, nb_xray_king_attacked);
                int arrive = first_index(attack);
                /*checkig for promotion*/
                if(attack != 0ULL){
                    if ((arrive/8 == 0 )| (arrive/8 == 7)){
                        add_move(moves, encode_move(position, arrive, i, 1, 0, 0, 0, 0));
                    }
                    else{
                        add_move(moves, encode_move(position, arrive, i, 0, 0, 0, 0, 0));
                    }
                }
                /*double movement*/
                U64 double_move = pawn_movements[turn][position + 8 * (1-2 * turn)] & ~occupancy[2];
                if ((1+turn*5 <= position/8) & (position/8 < 2 + turn * 5) & (attack!=0ULL) & (check_restriction(double_move, opponent_attack, opponent_attacks, nb_king_attacked, king_position) != 0ULL)){
                    if(pinned_restriction(double_move, position, opponent_xray_attack, nb_xray_king_attacked) != 0ULL){
                        arrive = first_index(double_move);
                        add_move(moves, encode_move(position, arrive, i, 0, 0, 1, 0, 0));   
                    }
                }
                /* attack movement*/
                U64 pawn_occupancy = occupancy[1 - turn];
                set_bit(pawn_occupancy, state[EP]);
                attack = (pawn_attacks[turn][position] & pawn_occupancy);
                attack = check_restriction(attack, opponent_attack, opponent_attacks, nb_king_attacked, king_position);
                attack = pinned_restriction(attack, position, opponent_xray_attack, nb_xray_king_attacked);
                /*en passant*/
                if (get_bit(attack, state[EP])!= 0ULL){
                    add_move(moves, encode_move(position, state[EP], i, 0, 1, 0, 1, 0));
                    pop_bit(attack, state[EP]);
                }
                /*promotion in attack*/
                while(attack != 0ULL){
                    arrive = first_index(attack);
                    if((arrive/8 == 0 )| (arrive/8 == 7)){
                        add_move(moves, encode_move(position, arrive, i, 1, 1, 0, 0, 0));
                    }
                    else{
                        add_move(moves, encode_move(position, arrive, i, 0, 1, 0, 0, 0));
                    }
                    pop_bit(attack, arrive);
                }
                break;
            case KNIGHT:
                attack = knight_attacks[position] & ~occupancy[turn];
                break;
            case BISHOP:
                attack = put_bishop_attacks(occupancy[2], position, bishops) & ~occupancy[i/6];
                break;
            case ROOK:
                attack = put_rook_attacks(occupancy[2], position, rooks) & ~occupancy[i/6];
                break;
            case QUEEN:
                attack = (put_bishop_attacks(occupancy[2], position, bishops) | put_rook_attacks(occupancy[2], position, rooks)) & ~occupancy[i/6];
                break;
            case KING:
                /* code */
                /*regular movement restained by adv attack*/
                attack = king_attacks[position] & ~opponent_attacks & ~occupancy[turn];
                /* castling move*/
                if (!is_attacked(king_position, opponent_attacks)){
                    if (state[2 + turn * 2]){
                        if((get_bit(occupancy[2], king_position+1) == 0ULL) & (get_bit(occupancy[2], king_position+2) == 0ULL)){
                            if (!(is_attacked(king_position+1, opponent_attacks) | is_attacked(king_position+2, opponent_attacks)) ){
                                add_move(moves, encode_move(position, (position+2), i, 0, 0, 0, 0, 1));
                            }
                        }
                    }
                    if (state[2 + turn * 2 + 1] ){
                        if((get_bit(occupancy[2], king_position-1) == 0ULL) & (get_bit(occupancy[2], king_position-2) == 0ULL)){
                            if (!(is_attacked(king_position-1, opponent_attacks) | is_attacked(king_position-2, opponent_attacks)) ){
                                add_move(moves, encode_move(position, (position-2), i, 0, 0, 0, 0, 1));
                            }
                        }
                    }
                }
                break;
            default:
                break;
            }
            if (i%6 != KING && attack != 0ULL){
                /* put restriction if player is checked*/
                attack = check_restriction(attack, opponent_attack, opponent_attacks, nb_king_attacked, king_position);
                /* put restriction if piece is pinned*/
                attack = pinned_restriction(attack, position, opponent_xray_attack, nb_xray_king_attacked);
            }
            /* encode the rest of the move*/
            while (attack != 0ULL){
                int arrive = first_index(attack);
                U64 capture = (1ULL << arrive) & occupancy[1-state[TURN]]; 
                if (capture != 0ULL){
                    add_move(moves, encode_move(position, arrive, i, 0, 1, 0, 0, 0));

                }
                else{
                    add_move(moves, encode_move(position, arrive, i, 0, 0, 0, 0, 0));
                }
                pop_bit(attack, arrive);
            }
            pop_bit(piece, position);
        }
    }
    if (moves->len == 0){
        if (is_attacked(king_position, opponent_attacks)){
            return -1;
        }
        return 0;
    }
    return 1;
}

/* move getter and maker */

int get_move(Moves* moves, int depart, int arrive, int piece){
    for(int i=0; i<moves->len; i++){
        if ((get_source(moves->moves[i]) == depart) & (get_target(moves->moves[i]) == arrive) & (get_piece(moves->moves[i]) == piece)){
            return moves->moves[i];
        }
    }
    return 0;
}

void make_move(U64 bitboard[12], U64 occupancy[3], int move, int state[6]){
    int depart = get_source(move);
    int arrive = get_target(move);
    int piece = get_piece(move);
    int capture = get_capture(move);
    int promoted = get_promoted(move);
    int en_double = get_double(move);
    int en_passant = get_enpassant(move);
    int castling = get_castling(move);
    if (capture){
        if (en_passant != 0){
            pop_bit(bitboard[PAWN + (1-state[0])*NB_PIECE], state[EP] - 8 * (1 - 2 * state[0]));

            pop_bit(occupancy[(state[TURN]?0:1)], state[EP] - 8 * (1 - 2 * state[0]));

            pop_bit(occupancy[2], state[EP] + 8 * (1 - 2 * state[0]));
        }
        else{
            pop_bit(bitboard[get_piece_from_position(bitboard, arrive)], arrive);
            pop_bit(occupancy[(state[TURN]?0:1)], arrive);

            pop_bit(occupancy[2], arrive);

        }
    }
    if(en_double != 0){
        state[EP] = (arrive - 8 * (1 - 2 * state[0]));
    }
    else{
        state[EP] = 0;
    }
    if (castling){
        if((arrive-depart) < 1){
            pop_bit(bitboard[ROOK + state[TURN]*NB_PIECE], arrive-1);
            set_bit(bitboard[ROOK + state[TURN]*NB_PIECE], arrive+1);

            pop_bit(occupancy[state[TURN]], arrive-1);
            set_bit(occupancy[state[TURN]], arrive+1);

            pop_bit(occupancy[2], arrive-1);
            set_bit(occupancy[2], arrive+1);
        }

        else{
            pop_bit(bitboard[ROOK + state[TURN]*NB_PIECE], arrive+2);
            set_bit(bitboard[ROOK + state[TURN]*NB_PIECE], arrive-1);

            pop_bit(occupancy[state[TURN]], arrive+2);
            set_bit(occupancy[state[TURN]], arrive-1);

            pop_bit(occupancy[2], arrive+2);
            set_bit(occupancy[2], arrive-1);
        }
        state[CWK+state[TURN]*2] = 0;
        state[CWQ+state[TURN]*2] = 0;
    }
    else if ((piece%6 == KING)){
        state[CWK+state[TURN]*2] = 0;
        state[CWQ+state[TURN]*2] = 0;
    }
    else if((piece%6 == ROOK) & (depart%56 == 0)) {
        state[CWK+state[TURN]*2] = 0;
    }
    else if ((piece%6==ROOK) & ((depart == 7) | (depart == 63))){
        state[CWQ+state[TURN]*2] = 0;
    }
    pop_bit(bitboard[piece], depart);

    pop_bit(occupancy[state[TURN]], depart);
    pop_bit(occupancy[2], depart);
    if (promoted){
        set_bit(bitboard[QUEEN+state[TURN]*6], arrive);

        set_bit(occupancy[state[TURN]], arrive);
        set_bit(occupancy[2], arrive);
    }
    else{
        set_bit(bitboard[piece], arrive);

        set_bit(occupancy[state[TURN]], arrive);
        set_bit(occupancy[2], arrive);
    }
   

    state[0] = state[0] ? WHITE: BLACK;
    }