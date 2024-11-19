#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_ 1

#include <MLV/MLV_all.h>

#include "pieces.h"

#define WIDTH 1200
#define HEIGHT 800
#define PADDINGX 50
#define PADDINGY 100

#define BOARD_SHAPE (2 * HEIGHT / 3)

#define CASE_SHAPE (BOARD_SHAPE / 8)

#define COLORW MLV_COLOR_SNOW
#define COLORB MLV_COLOR_DARK_SLATE_GRAY
#define COLOR_LETTER MLV_COLOR_SLATE_BLUE

#define NAME "chess/images/Chess_"

typedef struct
{
    int len;
    MLV_Image* images[32];
}Images;



//pieces sources: By en:User:Cburnett - File:Chess blt45.svg, CC BY-SA 3.0, https://commons.wikimedia.org/w/index.php?curid=20363777

void create_window(void);
Images* draw_board(U64 board[12]);

void free_images(Images* images);

int get_movement(U64 board[12], int* depart, int* arrive, int* piece);

#endif