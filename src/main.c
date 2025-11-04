#include <stdio.h>

#include <fight.h>

static void banner(void) {
    puts("==============================================");
    puts("      OceanDepth — Prototype console (v0.2)   ");
    puts("==============================================");
}

int main(void) {
    srand((unsigned)time(NULL));
    banner();

    Plongeur joueur;
    joueur_init(&joueur);

    int profondeur = 50; // zone de départ
    for (;;) {
        printf("\nMenu : 1-Descendre  2-Explorer  3-Remonter  4-Quitter\n");
        int ch = lire_entier_borne("> ", 1, 4);
        if (ch == 1) {
            profondeur += 50;
            if (profondeur > 300) profondeur = 300;
            printf("Vous descendez. Nouvelle profondeur: -%dm.\n", profondeur);
        } else if (ch == 2) {
            CreatureMarine ennemis[MAX_CREATURES] = {0};
            int nb = generer_creatures(ennemis, profondeur);
            printf("\nVous explorez... %d créature(s) approchent !\n", nb);
            IssueCombat res = lancer_combat(&joueur, ennemis, nb, profondeur);
            if (res == ISSUE_VICTOIRE) {
                int perles = rand_between(5,15) * nb;
                joueur.perles += perles;
                printf("\nVictoire ! Vous ramassez %d perles (total: %d).\n", perles, joueur.perles);
            } else if (res == ISSUE_DEFAITE) {
                puts("\nVous avez succombé dans les abysses... Fin de partie.");
                break;
            }
            if (joueur.pv <= 0) break;
        } else if (ch == 3) {
            profondeur -= 50;
            if (profondeur < 0) profondeur = 0;
            printf("Vous remontez. Profondeur: -%dm.\n", profondeur);
        } else {
            puts("Au revoir.");
            break;
        }
    }
    return 0;
}


