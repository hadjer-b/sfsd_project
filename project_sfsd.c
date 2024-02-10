/***
 * Ce programme a été écrit dans Visual Studio Code. 
 * Veuillez l'utiliser pour compiler le code, pour éviter les erreurs.
 * 
 * PROJECT SFSD 2eme annees Ingénieur
    - Berkani Hadjer du groupe 2
    - Touafek Malak du groupe 4
    - Redjradj Nicette du groupe 4

***/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define NB_GROUPES 8
#define NB_EQUIPES_PAR_GROUPE 4
#define NB_MATCHES_PAR_GROUPE ((NB_EQUIPES_PAR_GROUPE * (NB_EQUIPES_PAR_GROUPE - 1)) / 2)

typedef struct {
    int id;
    char nom[20];
    int points;
    int goals_for;
    int goals_against;
    int points_phase2;
} Equipe;

int id_total = 0;

typedef struct {
    Equipe equipes[NB_EQUIPES_PAR_GROUPE];
    int rencontres[NB_MATCHES_PAR_GROUPE][2];
} Groupe;

// Structure representing a node of the binary tree
typedef struct Noeud {
    Equipe equipe1;
    Equipe equipe2;
    struct Noeud *gauche;
    struct Noeud *droite;
} Noeud;

Equipe* WINNER = NULL;

void initialiser_equipes(Equipe equipes[], int nb_equipes, FILE* liste_equipes) {
    char  str[50];
    int i = 0;
    for (i = 0; i < nb_equipes; i++) {
        equipes[i].id = id_total + 1;
        equipes[i].points = 0;
        equipes[i].goals_for = 0;
        equipes[i].goals_against = 0;
        equipes[i].points_phase2 = 0;
        if (fscanf(liste_equipes,"%s", str)) {
            strcpy(equipes[i].nom , str);
        }
        id_total += 1;
    }
}

void generer_rencontres(Groupe groupes[], int nb_groupes) {
    int g = 0;
    for (g = 0; g < nb_groupes; g++) {
        int match_index = 0;
        for (int i = 0; i < NB_EQUIPES_PAR_GROUPE - 1; i++) {
            for (int j = i + 1; j < NB_EQUIPES_PAR_GROUPE; j++) {
                groupes[g].rencontres[match_index][0] = i;
                groupes[g].rencontres[match_index][1] = j;
                match_index++;
            }
        }
    }
}

void simuler_rencontre_hasard(Equipe* equipe1, Equipe* equipe2) {
    int score1 = rand() % 8;
    int score2 = rand() % 8;

    if (score1 > score2) {
        equipe1->points += 3;
    } else if (score1 < score2) {
        equipe2->points += 3;
    } else {
        equipe1->points += 1;
        equipe2->points += 1;
    }

    printf("%s vs %s : %d - %d\n", equipe1->nom, equipe2->nom, score1, score2);
}

void simuler_rencontre_favorite(Equipe* equipe1, Equipe* equipe2) {
    int score_win = (rand() % (7)) + 1;
    int score_lose = (rand() % ((score_win - 1) + 1));

    if (equipe1->id > equipe2->id) {
        equipe1->points += 3;
    } else if (equipe1->id < equipe2->id) {
        equipe2->points += 3;
    }

    printf("%s vs %s : %d - %d\n", equipe1->nom, equipe2->nom, score_win, score_lose);
}

void simuler_rencontre_mixte(Equipe* equipe1, Equipe* equipe2) {
    if (rand() % 2 == 0) {
        simuler_rencontre_hasard(equipe1, equipe2);
    } else {
        simuler_rencontre_favorite(equipe1, equipe2);
    }
}

void simuler_phase1(Groupe groupes[], int nb_groupes, Equipe premiers[], Equipe seconds[], char choix) {
    int i = 0; int j = 0; int k = 0;
    for (i = 0; i < nb_groupes; i++) {
        printf("Groupe %d :\n", i + 1);
        for (j = 0; j < NB_MATCHES_PAR_GROUPE; j++) {
            int equipe1_index = groupes[i].rencontres[j][0];
            int equipe2_index = groupes[i].rencontres[j][1];
            printf("Match %d: ", j + 1);

            switch (choix) {
            case '1':
                simuler_rencontre_hasard(&groupes[i].equipes[equipe1_index], &groupes[i].equipes[equipe2_index]);
                break;
            case '2':
                simuler_rencontre_favorite(&groupes[i].equipes[equipe1_index], &groupes[i].equipes[equipe2_index]);
                break;
            case '3':
                simuler_rencontre_mixte(&groupes[i].equipes[equipe1_index], &groupes[i].equipes[equipe2_index]);
                break;
            }
        }

        printf("Standings:\n");
        for (k = 0; k < NB_EQUIPES_PAR_GROUPE; k++) {
            printf("%s : %d points\n", groupes[i].equipes[k].nom, groupes[i].equipes[k].points);
        }
        printf("\n");

        Equipe* equipe_premiere = &groupes[i].equipes[0];
        Equipe* equipe_deuxieme = &groupes[i].equipes[1];

        for (int j = 2; j < NB_EQUIPES_PAR_GROUPE; j++) {
            if (groupes[i].equipes[j].points > equipe_premiere->points) {
                equipe_deuxieme = equipe_premiere;
                equipe_premiere = &groupes[i].equipes[j];
            } else if (groupes[i].equipes[j].points > equipe_deuxieme->points) {
                equipe_deuxieme = &groupes[i].equipes[j];
            }
        }

        premiers[i] = *equipe_premiere;
        seconds[i] = *equipe_deuxieme;
        printf("Appuyez sur Entre pour voir le resultas du groupe %d", i + 2);
        getchar(); 
        system("cls");
    }
}

Noeud* creerNoeud(Equipe equipe1, Equipe equipe2) {
    Noeud* nouveauNoeud = (Noeud*)malloc(sizeof(Noeud));
    nouveauNoeud->equipe1 = equipe1;
    nouveauNoeud->equipe2 = equipe2;
    nouveauNoeud->gauche = NULL;
    nouveauNoeud->droite = NULL;
    return nouveauNoeud;
}

Noeud* creeArbreVide(int levels) {
    if (levels == 0)
        return NULL;

    Equipe equipe_vide;
    equipe_vide.id = -1;
    strcpy(equipe_vide.nom, "vide");
    equipe_vide.points = 0;
    equipe_vide.goals_against = 0;
    equipe_vide.goals_for = 0;
    equipe_vide.points_phase2 = 0;

    Noeud* root = creerNoeud(equipe_vide, equipe_vide);

    root->gauche = creeArbreVide(levels - 1);
    root->droite = creeArbreVide(levels - 1);

    return root;
}

Noeud* findNode(Noeud* root, int level, int position) {
    if (root == NULL || level < 0 || position < 0)
        return NULL;
    if (level == 0 && position == 0)
        return root;

    int maxPosition = pow(2, level) - 1;
    if (position > maxPosition)
        return NULL;

    int mid = maxPosition / 2;
    if (position <= mid)
        return findNode(root->gauche, level - 1, position);
    else
        return findNode(root->droite, level - 1, position - mid - 1);
}

void afficherArbre(Noeud* racine, int niveau) {
    if (racine == NULL) {
        return;
    }
    int i = 0;
    for (i = 0; i < niveau; i++) {
        printf("\t");
    }
    printf("(%s vs %s) niveau:%d \n", racine->equipe1.nom, racine->equipe2.nom, niveau);
    afficherArbre(racine->gauche, niveau + 1);
    afficherArbre(racine->droite, niveau + 1);
}

void affichageTournoie(Noeud* racine) {
    int j = 0; int i = 0;
    for (i = 0; i < 8; i++)
    {
        printf("\n");
        Noeud* level_3_node = findNode(racine, 3, i);
        printf("    %s\n", level_3_node->equipe1.nom);
        if (i % 2 == 0)
        {
            Noeud* level_2_node = findNode(racine, 2, j);
            printf("|-----------> %s\n", level_2_node->equipe1.nom);
            
        }
        else if (i % 2 != 0)
        {
            Noeud* level_2_node = findNode(racine, 2, j);
            printf("|-----------> %s\n", level_2_node->equipe2.nom);
            j++;
        }

        printf("    %s\n", level_3_node->equipe2.nom);

        if (i == 0)
        {
            Noeud* level_1_node = findNode(racine, 1, 0);
            printf("|-------------------------->%s", level_1_node->equipe1.nom);
        }
        else if (i == 1)
        {
            printf("|\n");
            printf("|---------------------------------------> %s\n", racine->equipe1.nom);
            printf("|");
        }
        else if (i == 2)
        {
            Noeud* level_1_node = findNode(racine, 1, 0);
            printf("|-------------------------->%s", level_1_node->equipe2.nom);
        }
        else if (i == 3){
            printf("\t\t\t\t\t\t-----> |%s|\n", WINNER->nom);
        }
        else if (i == 4)
        {
            Noeud* level_1_node = findNode(racine, 1, 1);
            printf("|-------------------------->%s", level_1_node->equipe1.nom);
        }
        else if (i == 5){
            printf("|\n");
            printf("|---------------------------------------> %s\n", racine->equipe2.nom);
            printf("|");
        }
        else if (i == 6)
        {
            Noeud* level_1_node = findNode(racine, 1, 1);
            printf("|--------------------------> %s", level_1_node->equipe2.nom);
        }
    }

}

void afficherArbreToFile(Noeud* racine, int niveau, FILE* file) {
    if (racine == NULL) {
        return;
    }
    int i = 0;
    for (i = 0; i < niveau; i++) {
        fprintf(file, "\t");
    }
    fprintf(file, "(%s vs %s) %d \n", racine->equipe1.nom, racine->equipe2.nom, niveau);
    afficherArbreToFile(racine->gauche, niveau + 1, file);
    afficherArbreToFile(racine->droite, niveau + 1, file);
}

Equipe* simuler_rencontre_hasard_phase2(Equipe* equipe1, Equipe* equipe2) {
    int score1 = rand() % 8;
    int score2 = rand() % 8;

    if (score1 == score2) {
        simuler_rencontre_hasard_phase2(equipe1, equipe2);
    }

    if (score1 > score2) {
        return equipe1;
    } else {
        return equipe2;
    }
}

Equipe* simuler_favorie_phase2(Equipe* equipe1, Equipe* equipe2) {
    if (equipe1->id > equipe2->id) {
        return equipe1;
    } else {
        return equipe2;
    }
}

Equipe* simuler_selon_phase1(Equipe* equipe1, Equipe* equipe2) {
    if (equipe1->points > equipe2->points) {
        return equipe1;
    } else {
        return equipe2;
    }
}

Equipe* simuler_selon_phase1_phase2(Equipe* equipe1, Equipe* equipe2) {
    int score1 = rand() % 8;
    int score2 = rand() % 8;
    equipe1->points_phase2 += score1;
    equipe2->points_phase2 += score2;

    if (equipe1->points > equipe2->points && equipe1->points_phase2 >= equipe2->points_phase2) {
        return equipe1;
    } else {
        return equipe2;
    }
}

Equipe* simuler_selon_phase1_favorie(Equipe* equipe1, Equipe* equipe2) {
    if (equipe1->id > equipe2->id && equipe1->points >= equipe2->points) {
        return equipe1;
    } else {
        return equipe2;
    }
}


void simuler_les_autres_niveau(Noeud* root,int nbr_match,  int level, char choix) {
    int j = 0; int i = 0;
    for (i = 0; i < nbr_match; i += 2) {
        Noeud* X1 = findNode(root, level + 1, i);
        Noeud* X2 = findNode(root, level + 1, i + 1);
        Equipe* winner_1;
        Equipe* winner_2;

        switch (choix)
        {
            case '1':
                winner_1 = simuler_rencontre_hasard_phase2(&X1->equipe1, &X1->equipe2);
                winner_2 = simuler_rencontre_hasard_phase2(&X2->equipe1, &X2->equipe2);
                break;
            case '2':
                winner_1 = simuler_favorie_phase2(&X1->equipe1, &X1->equipe2);
                winner_2 = simuler_favorie_phase2(&X2->equipe1, &X2->equipe2);
                break;
            case '3':
                winner_1 = simuler_selon_phase1(&X1->equipe1, &X1->equipe2);
                winner_2 = simuler_selon_phase1(&X2->equipe1, &X2->equipe2);
                break;
            case '4':
                winner_1 = simuler_selon_phase1_phase2(&X1->equipe1, &X1->equipe2);
                winner_2 = simuler_selon_phase1_phase2(&X2->equipe1, &X2->equipe2);
                break;
            case '5':
                winner_1 = simuler_selon_phase1_favorie(&X1->equipe1, &X1->equipe2);
                winner_2 = simuler_selon_phase1_favorie(&X2->equipe1, &X2->equipe2);
                break;
        }
        

        Noeud* place = findNode(root, level, j );
        place->equipe1 = *winner_1;
        place->equipe2 = *winner_2;
        j++;
    }
}

void simulerPhase2(Noeud* root, Equipe premiers[], Equipe seconds[], char choix) {
    // Les huitiemes de finale
    int level = 3;
    int j = 0;
    int i = 0;
    for ( i = 0; i < 8; i += 2) {
        Noeud* found =  findNode(root, level, j);
        found->equipe1 = premiers[i];
        found->equipe2 = seconds[i + 1];
        j += 1;
    }

    for (i = 1; i < 8; i += 2) {
        Noeud* found =  findNode(root, level, j);
        found->equipe1 = premiers[i];
        found->equipe2 = seconds[i - 1];
        j += 1;
    }

    // simuler le niveau 2 qui a 8 match ( quart des finale )
    simuler_les_autres_niveau(root, 8, 2, choix);
    // simuler le niveau 1 qui a 4 match ( demi-finale)
    simuler_les_autres_niveau(root, 4, 1, choix);
    // simuler le niveau 0 qui a 2 match (finale)
    simuler_les_autres_niveau(root, 2, 0, choix);

    switch (choix)
    {
        case '1':
            WINNER = simuler_rencontre_hasard_phase2(&root->equipe1, &root->equipe2);
            break;
        case '2':
            WINNER = simuler_favorie_phase2(&root->equipe1, &root->equipe2);
            break;
        case '3':
            WINNER = simuler_selon_phase1(&root->equipe1, &root->equipe2);
            break;
        case '4':
            WINNER = simuler_selon_phase1_phase2(&root->equipe1, &root->equipe2);
            break;
        case '5':
            WINNER = simuler_selon_phase1_favorie(&root->equipe1, &root->equipe2);
            break;
    }
}

void WelcomeWindow() {
    printf("                                                                                                    _____      \n");
    printf("                                                  .-''-.                          .-''-.           /    /      \n");
    printf("          .--.                                  .' .-.  )                       .' .-.  )         /    /       \n");
    printf("     _.._ |__|     _.._                        / .'  / /                       / .'  / /         /    /        \n");
    printf("   .' .._|.--.   .' .._|                      (_/   / /         .-''` ''-.    (_/   / /         /    /         \n");
    printf("   | '    |  |   | '       __                      / /        .'          '.       / /         /    /  __      \n");
    printf(" __| |__  |  | __| |__  .:--.`.                   / /        /              `     / /         /    /  |  |     \n");
    printf("|__   __| |  ||__   __|/ |   \\ |                 . `        `                `   . `         /    `   |  |     \n");
    printf("   | |    |  |   | |   `` __ | |                / /    _.-')|         .-.    |  / /    _.-')/    `----|  |---. \n");
    printf("   | |    |__|   | |    .'.''| |              .' '  _.'.-'' .        |   |   ..' '  _.'.-''/          |  |   | \n");
    printf("   | |           | |   / /   | |_            /  /.-'_.'      .       '._.'  //  /.-'_.'    '----------|  |---' \n");
    printf("   | |           | |   \\ \\._,\\ '/           /    _.'          '._         .'/    _.'                  |  |     \n");
    printf("   |_|           |_|    `--'  `\"           ( _.-'                '-....-'` ( _.-'                    /____\\    \n");

    printf("\nProject SFSD realise par les etudiants 2eme annees ingenieur:\n");
    printf("- Berkani Hadjer du groupe 2\n");
    printf("- Touafek Malak du groupe 4\n");
    printf("- Redjradj Nicette du groupe 4\n");
    printf("\n Appuyez sur entrer pour comencer:\n");

    //Lire un character de l'utilisateur sans l'enregistrer dans un buffer
    getchar();
    getchar();
    //Effacer l'ecrant
    system("cls");
}

void voirListeFavorie(){
    char choix[10];
    printf("Voulez vous voir la liste des equipes favorites? (y pour Oui, n pour Non):\n");
    do {
        printf("Votre choix: ");
        scanf("%s", choix);
    } while (choix[0] != 'y' && choix[0] != 'n');

    if (choix[0] == 'y')
    {
        printf("liste des favories (du moins favorie jusqu'au plus favorie):\n");
        FILE* liste_equipes = fopen("liste_equipes.txt", "r");
        char  str[50];
        int i = 32;
        while(fscanf(liste_equipes,"%s", str) == 1){
            printf("%d- %s\n",i,str);
            i--;
        }
        fclose(liste_equipes);
    }
    
    
    printf("\nAppuyez sur entrer pour comencer la simulation:\n");
    getchar();
    getchar();
    system("cls");
}

void freeTree(Noeud *root) {
    if (root == NULL) {
        return;
    }
    // Liberer l'arbre guache
    freeTree(root->gauche);
    // Free the right subtree
    freeTree(root->droite);
    // Free the current node
    free(root);
}

char interface_phase1() {
    printf("                                                                                        ,/)\n");
    printf("                                                                                        |_|\n");
    printf("        _        _        _        _        _        _        _        _        _       ].[       ,~, \n");
    printf("       |.|      |.|      |.|      |.|      |.|      |.|      |.|      |.|      |.|    /~`-'~\\     |_|\n");
    printf("       ]^[      ]^[      ]^[      ]^[      ]^[      ]^[      ]^[      ]^[      ]^[   (<| 11 |>)   ]0[\n");
    printf("     /~/^\\~\\  /~`-'~\\  /~`-'~\\  /~`-'~\\  /~`-'~\\  /~`-'~\\  /~`-'~\\  /~`-'~\\  /~`-'~\\  \\|___|/   ,-`^'~\\\n");
    printf("    {<| 1 |>}{<| 2 |>}{<| 3 |>}{<| 4 |>}{<| 5 |>}{<| 7 |>}{<| 8 |>}{<| 9 |>}{<| 0 |>} {/   \\} {<| 12  |>}\n");
    printf("     \\|___|/  \\|___|/  \\|___|/  \\|___|/  \\|___|/  \\|___|/  \\|___|/  \\|___|/  \\|___|/  /__1__\\   \\|,__|/\n");
    printf("    /\\    \\    /   \\    /   \\    /   \\    /   \\    /   \\    /   \\    /   \\    /   \\   | / \\ |   {/ \\  /\n");
    printf("    |/>/|__\\  /__|__\\  /__|__\\  /__|__\\  /__|__\\  /__|__\\  /__|__\\  /__|__\\  /__|__\\  |/   \\|   /__|\\/\\\n");
    printf("    |)   \\ |  | / \\ |  | / \\ |  | / \\ |  | / \\ |  | / \\ |  | / \\ |  | / \\ |  | / \\ |  {}   {}   | / \\ |\n");
    printf("   (_|    \\)  (/   \\)  (/   \\)  (/   \\)  (/   \\)  (/   \\)  (/   \\)  (/   \\)  (/   \\)  |)   (|   (/   \\)\n");
    printf("   / \\    (|  |)   (|  |)   (|  |)   (|  |)   (|  |)   (|  |)   (|  |)   (|  |)   (|  ||   ||  _|)   (|_\n");
    printf(".,.\\_/,..,|,)(.|,.,|,)(,|,.,|.)(.|,.,|,)(,|,.,|.)(.|,.,|,)(,|,.,|.)(.|,.,|,)(,|,.,|.)(.|.,.|,)(.,|.,.|,.),.,.\n\n");

    printf("---------------------------------------------------------------------------------------\n");
    printf("------------------------------------   PHASE I  ----------------------------------------\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("Choisir le type de rencontre pour la Phase I:\n");
    printf("[1]: Au hasard\n");
    printf("[2]: Selon les equipes favorites\n");
    printf("[3]: Selon le hasard ET les equipes favorites\n");
    char choix[10];
    do {
        printf("Votre choix: ");
        scanf("%s", choix);
    } while (choix[0] < '1' || choix[0] > '3');
    return choix[0];
    system("cls");
}

char interface_phase2() {
    printf("---------------------------------------------------------------------------------------\n");
    printf("------------------------------------   PHASE II  ----------------------------------------\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("Choisir le type de rencontre pour la Phase II:\n");
    printf("[1]: Au hasard\n");
    printf("[2]: Selon les favoris\n");
    printf("[3]: Selon resultas de la phase 1\n");
    printf("[4]: Selon les resultats de phase 1 et la phase 2\n");
    printf("[5]: Selon les favorie et resultas de la phase 1\n");
    char choix[10];
    do {
        printf("Votre choix: ");
        scanf("%s", choix);
    } while (choix[0] < '1' || choix[0] > '5');

    return choix[0];
}

void afficherWinner(){

    printf("\n\n");
    printf("\t____    __    ____  __  .__   __.  .__   __.  _______ .______      \n");
    printf("\t\\   \\  /  \\  /   / |  | |  \\ |  |  |  \\ |  | |   ____||   _  \\     \n");
    printf("\t \\   \\/    \\/   /  |  | |   \\|  |  |   \\|  | |  |__   |  |_)  |    \n");
    printf("\t  \\            /   |  | |  . `  |  |  . `  | |   __|  |      /     \n");
    printf("\t   \\    /\\    /    |  | |  |\\   |  |  |\\   | |  |____ |  |\\  \\----.\n");
    printf("\t    \\__/  \\__/     |__| |__| \\__|  |__| \\__| |_______|| _| `._____|\n\n");
    printf("\t\t                @@@@@@@@@@@@@@@                \n");
    printf("\t\t             @@@@@@@           @@@            \n");
    printf("\t\t           @@@@@@@@@            @@@@          \n");
    printf("\t\t         @@ @@@     @          @    @@        \n");
    printf("\t\t        @   @         @      @@       @       \n");
    printf("\t\t       @   @          @@@@@@@@@        @      \n");
    printf("\t\t      @   @@          @@@@@@@@@@        @     \n");
    printf("\t\t     @    @          @@@@@@@@@@@        @@    \n");
    printf("\t\t     @   @@@        @@@@@@@@@@@@@        @    \n");
    printf("\t\t     @ @@@@@@    @@   @@@@@@@@@   @@    @@    \n");
    printf("\t\t     @@@@@@@@@@@         @@@         @@@@@    \n");
    printf("\t\t     @@@@@@@@@            @           @@@@    \n");
    printf("\t\t     @ @@@@@@@            @           @@@@    \n");
    printf("\t\t      @@@@@@@@           @@          @@@@     \n");
    printf("\t\t       @@@@@ @@          @          @@ @@     \n");
    printf("\t\t        @@      @@     @@@@@     @@   @@      \n");
    printf("\t\t         @         @@@@@@@@@@@@@     @        \n");
    printf("\t\t           @@       @@@@@@@@@@     @          \n");
    printf("\t\t             @@@    @@@@@@@@@  @@@            \n");
    printf("\t\t                 @@@@@@@@@@@@@@                \n");

    printf("\n|--------------------> Le gagnant de la fifa est: %s \n", WINNER->nom);

    getchar();
    system("cls");
}

int main() {
    srand(time(NULL));
    // changer la couleur de la terminale
    system("COLOR 75");

    Groupe groupes[NB_GROUPES];
    Equipe premiers[NB_GROUPES];
    Equipe seconds[NB_GROUPES];    

    //initialiser les equipes et rencontre
    FILE* liste_quipes = fopen("liste_equipes.txt", "r");
    for (int i = 0; i < NB_GROUPES; i++) {
        initialiser_equipes(groupes[i].equipes, NB_EQUIPES_PAR_GROUPE, liste_quipes);
        generer_rencontres(groupes, NB_GROUPES);
    }
    fclose(liste_quipes);

    //Les ecrants de demarage
    WelcomeWindow();

    voirListeFavorie();

    // LA PHASE I
    char choix = interface_phase1();
    
    simuler_phase1(groupes, NB_GROUPES, premiers, seconds, choix);

    //Afficher resultas de la phase I
    printf("Equipes retenues pour la Phase II :\n");
    int i = 0;
    for (i = 0; i < NB_GROUPES; i++) {
        printf("Premier du groupe %d : %s\n", i + 1, premiers[i].nom);
    }
    printf("\n");
    for (i = 0; i < NB_GROUPES; i++) {
        printf("Second du groupe %d : %s\n", i + 1, seconds[i].nom);
    }

    printf("\nAppuyez sur Enter pour passer a la phase suivante: \n");
    getchar(); 
    system("cls");

    //Phase II
    choix = interface_phase2();

    Noeud* root = creeArbreVide(4);
    simulerPhase2(root, premiers, seconds, choix);
    system("cls");

    //Afficher les resultas de la phase II
    affichageTournoie(root);

    getchar();
    getchar();
    getchar();
    system("cls");

    //Vainceur
    afficherWinner();

    //Afficher l'arbre a l'utilisateur
    printf("Voulez-vous voir l'arbre binaire? (y pour Oui, n pour Non): ");
    char choix_arbre[20];
    do {
        printf("\nVotre choix: ");
        scanf("%s", choix_arbre);
    } while (choix_arbre[0] != 'y' && choix_arbre[0] != 'n');

    if (choix_arbre[0] == 'y') {
        printf("Affichage de l'arbre pour la Phase II: \n\n");
        afficherArbre(root, 0 );
        printf("\n\n\n");
    }

    //Enregistrer l'arbre dans fichier
    FILE* file = fopen("arbre_phase2.txt", "w");
    if (file != NULL) {
        fprintf(file, "Arbre pour la Phase II: \n");
        afficherArbreToFile(root, 0, file);
        fclose(file);
        printf("L'arbre a ete ecrite dans le fichier 'arbre_phase2.txt' qui ce trouve dans le dossier output.\n");
    } else {
        printf("Erreur lors de l'ouverture du fichier pour écrire l'arbre.\n");
    }

    freeTree(root);
    return 0;
}