#include <stdio.h>

#include "games.h"

int main(int argc, char* argv[]){
    if(argc == 1){
        char start[]  =  "rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR w";
        int choix;
        char player[30];
        //char test[] = "1kr1q2r/2p2p2/5Q1p/2p1p1p1/1n6/4P3/PP1P1PPP/R1BK2NR w";
        printf("Vous avez choisi de jouer\n");
        printf("Voulez-vous jouer contre un joueur ou contre un modèle?\n");
        printf("0:jeu seul\n1: contre modèle ia\n2:Regarder l'ia affronter l'ordinateur\n");
        scanf("%d", &choix);
        getchar();
        if(choix==1){
            printf("Choisissez le modèle a utilliser: \n");
            scanf("%s", player);
        }
        else if(choix == 2){
            printf("Choisissez le modèle à utiliser: \n");
            scanf("%s", player);
        }
        init_game(start, choix, player);
    }
    else{
        if (strcmp(argv[1], "data") == 0){
            int mode;
            char player[30];
            printf("Vous avez choisi le constructeur de data des joueurs\n");
            printf("Choisissez le mode:\n1: ajouter un joueur à la database\n2: créer une database\n3: afficher la listes des joueurs disponibles\n");
            printf("Choix: ");
            scanf("%d", &mode);
            getchar();
            if(mode == 1){
                printf("Vous avez choisi d'ajouter un joueur.\nVeuillez saisir le nom du joueur: ");
                scanf("%[^\n]%*c", player);
                printf("Création de la database du joueur %s\n", player);
                create_data(player);
            }
            else if(mode == 2){
                printf("Vous avez choisi d'ajouter l'ouverture joueur.\nVeuillez saisir le nom du joueur: ");
                scanf("%[^\n]%*c", player);
                printf("Création de la database du joueur %s\n", player);
                create_data_opening(player);
            }
            else if (mode == 3){
                show_names();
            }
        }
        if(strcmp(argv[1], "ai") == 0){
            int mode;
            char player[30];
            printf("Vous avez choisi le constructeur d'ia des joueurs\n");
            printf("Choisissez le mode:\n1: ajouter un joueur au model.\nIl faut que vous ayez construit la database du joueur!\n2: afficher la listes des joueurs disponibles\n");
            printf("Choix: ");
            scanf("%d", &mode);
            getchar();
            if(mode == 1){
                printf("Vous avez choisi d'ajouter un joueur.\nVeuillez saisir le nom du joueur: ");
                scanf("%[^\n]%*c", player);
                printf("Création du modèle du joueur %s\n", player);
                create_model(player);
                printf("Fin de la création du modèle du joueur\n");
            }
            else if (mode == 2){
                show_names_data();
            }
        }
    }
    return 0;
}