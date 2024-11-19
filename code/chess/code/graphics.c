#include "graphics.h"

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

void pixel_vers_case(int* x){
    *x /= CASE_SHAPE;
}

void case_vers_pixel(int* x){
    *x *= CASE_SHAPE;
}

void hightlight_click(int x, int y){
    int casex = x;
    int casey = y;
    case_vers_pixel(&casex);
    case_vers_pixel(&casey);
    MLV_draw_rectangle(casex+PADDINGX, casey, CASE_SHAPE, CASE_SHAPE, MLV_COLOR_YELLOW1);
    MLV_actualise_window();
}

void get_mouse_click(int* x, int* y){
    MLV_wait_mouse(x, y);
}

int in_board(int x, int y){
    if ((x > PADDINGX )& (x < PADDINGX+BOARD_SHAPE)){
        if ((y > 0) & (y < BOARD_SHAPE)){
            return 1;
        }
    }
    return 0;
}

void create_window(void){
    MLV_create_window("test", "test", WIDTH, HEIGHT);
}

Images* malloc_images(void){
    Images* images = malloc(sizeof(Images));
    images->len = 0;
    return images;
}

void add_images(Images* images, MLV_Image* image){
    images->images[images->len] = image;
    images->len++;
}

void free_images(Images* images){
    for(int i=0; i<images->len; i++){
        MLV_free_image(images->images[i]);
    }
    free(images);
}

MLV_Image* draw_piece(int x, int y, char piece, char color){
    char name[30] = NAME;
    char piece2 = piece;
    char c = (color == 'w')? 'l':'d';
    char* end = "t60.png";
    strncat(name, &piece2, 1);
    strncat(name, &c, 1);
    strncat(name, end, 7);
    MLV_Image* image = MLV_load_image(name);
    MLV_resize_image(image, CASE_SHAPE, CASE_SHAPE);
    MLV_draw_image(image, x, y);
    return image;
}

char from_int_to_piece(int i){
    switch(i%6){
        case PAWN:
            return 'p';
        case KNIGHT:
            return 'n';
        case BISHOP:
            return 'b';
        case ROOK:
            return 'r';
        case QUEEN:
            return 'q';
        case KING:
            return 'k';
    }
    return 'x';
}

Images* draw_all_pieces(U64 board[12]){
    int x, y;
    Images* images = malloc_images();
    for(int i=0; i<12; i++){
        U64 tmp = board[i];
        while(tmp != 0ULL){
            int position = first_index(tmp);
            x = position%8;
            y = position/8;
            case_vers_pixel(&x);
            case_vers_pixel(&y);
            char piece = from_int_to_piece(i);
            char color = i/6?'b':'w';
            add_images(images, draw_piece(x+PADDINGX, y, piece, color));
            pop_bit(tmp, position);
        }
    }
    return images;
}

Images* draw_board(U64 board[12]){
    MLV_clear_window(MLV_COLOR_BLACK);
    for(int i=0; i<8; i++){
        char l = (i+1) + '0';
        char c[1] = {l};
        MLV_draw_text(PADDINGX / 2, (i * CASE_SHAPE) + CASE_SHAPE/2, c, COLOR_LETTER, MLV_TEXT_CENTER);
        for(int j=0; j<8; j++){
            int vx[4] = {PADDINGX + CASE_SHAPE * j, PADDINGX + CASE_SHAPE * (j+1), PADDINGX + CASE_SHAPE * (j+1), PADDINGX + CASE_SHAPE * j};
            int vy[4] = {CASE_SHAPE * i, CASE_SHAPE * i, CASE_SHAPE * (i+1), CASE_SHAPE * (i+1)};
            MLV_Color  color = (i%8+j)%2? COLORB: COLORW;
            MLV_draw_filled_polygon(vx, vy, 4, color);
        }
    }
    for(int i=0; i<8; i++){
        char l = 97+i;
        char c[1] = {l};
        MLV_draw_text(PADDINGX + ((i+1) * CASE_SHAPE)-CASE_SHAPE/2, 8 * CASE_SHAPE + CASE_SHAPE/2, c, COLOR_LETTER, MLV_TEXT_CENTER);
    }
    Images* images = draw_all_pieces(board);
    MLV_actualise_window();
    return images;
}

int get_movement(U64 board[12], int* depart, int* arrive, int* piece){
    int x1, y1;
    *piece = -1;
    get_mouse_click(&x1, &y1);
    if (in_board(x1, y1)){
        pixel_vers_case(&x1);
        x1-=1;
        pixel_vers_case(&y1);
        hightlight_click(x1, y1);
        for(int i=0; i<12; i++){
            if (get_bit(board[i], (x1+y1*8)) != 0ULL){
                *piece = i;
                *depart = (x1+y1*8);
            }
        }
        
        if (*piece == -1){
            draw_board(board);
            return 0;
        }

    }
    int x2, y2;
    get_mouse_click(&x2, &y2);
    if (in_board(x2, y2)){
        pixel_vers_case(&x2);
        pixel_vers_case(&y2);
        x2-=1;
        *arrive = (x2+y2*8);
        draw_board(board);
        return 1;
    }
    draw_board(board);
    return 0;
}