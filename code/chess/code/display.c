#include <stdio.h>
#include "display.h"


void print_bits(U64 num){
    U64 i;
    for(i=1ULL << 63; i>0; i = i/2){
        (num & i) ? printf("1") : printf("0");
    }
    printf("\n");

}

char from_position_to_str(int position){
    switch (position){
    case PAWN:
        return 'p';
    case ROOK:
        return 'r';
    case KNIGHT:
        return 'n';
    case BISHOP:
        return 'b';
    case QUEEN:
        return 'q';
    case KING:
        return 'k';
    case NB_PIECE+PAWN:
        return 'P';
    case NB_PIECE+ROOK:
        return 'R';
    case NB_PIECE+KNIGHT:
        return 'N';
    case NB_PIECE+BISHOP:
        return 'B';
    case NB_PIECE+QUEEN:
        return 'Q';
    case NB_PIECE+KING:
        return 'K';
    default:
        return 'x';
    }   

}

void show_boards(U64 board[12]){
    printf("  ");
    for(int i=0; i<8; i++){
        char letter = 97+i;
        printf("%c ", letter);
    }
    printf("\n");
    for(int i = 0; i<8; i++){
        printf("\n");
        printf("%d ", i+1); 
        for (int j = 0; j<8; j++){
            bool piece = false;
            U64 bit = 0;
            add_bit(&bit, j+i*8);
            for (int l = 0; l<12; l++){
                if ((board[l]&bit) == bit){
                    piece = true;
                    printf("%c ", from_position_to_str(l));
                }
                //printf("%d, %lld\n", board[k]&bit, bit);
            }
            if (!piece){
                printf("x ");
            }
        }
    }
    printf("\n");
}

void show_board(U64 board){    
    for(int i = 0; i<8; i++){
        printf("\n"); 
        for (int j = 0; j<8; j++){
            (board & 1ULL << (i*8+j)) ? printf("1") : printf("0");
        }
    }
    printf("\n");
}


