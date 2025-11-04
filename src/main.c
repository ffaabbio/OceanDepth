#include <fight.h>
#include <inventory.h>
#include <save.h>

static void banner(void) {
    puts("==============================================");
    puts("      OceanDepth - v0.4 (save/load)           ");
    puts("==============================================");
}

static int peut_sauver(int profondeur, int en_grotte) {
    return (profondeur == 0) || en_grotte;
}

int main(void) {
    srand((unsigned)time(NULL));
    banner();

    Plongeur joueur; joueur_init(&joueur);

    int profondeur = 50; // zone de départ
    int en_grotte  = 0;  // 0 = non, 1 = dans une grotte (point sûr)

    for (;;) {
        printf("\nMenu : 1-Descendre  2-Explorer  3-Remonter  4-Inventaire  5-Grotte ON/OFF  6-Sauver  7-Charger  8-Quitter\n");
        int ch = lire_entier_borne("> ", 1, 8);
        if (ch == 1) {
            if (en_grotte) { puts("Vous devez sortir de la grotte avant de vous déplacer."); continue; }
            profondeur += 50; if (profondeur > 300) profondeur = 300;
            printf("Vous descendez. Profondeur: -%dm.\n", profondeur);
        } else if (ch == 2) {
            if (en_grotte) { puts("Vous êtes dans une grotte (zone sûre). Sortez pour explorer."); continue; }
            CreatureMarine ennemis[MAX_CREATURES] = {0};
            int nb = generer_creatures(ennemis, profondeur);
            printf("\nVous explorez... %d créature(s) approchent !\n", nb);
            IssueCombat res = lancer_combat(&joueur, ennemis, nb, profondeur);
            if (res == ISSUE_VICTOIRE) {
                puts("\nVictoire !");
                distribuer_recompenses(&joueur, ennemis, nb, profondeur);
            } else if (res == ISSUE_DEFAITE) {
                puts("\nVous avez succombé dans les abysses... Fin de partie.");
                break;
            }
            if (joueur.pv <= 0) break;
        } else if (ch == 3) {
            if (en_grotte) { puts("Vous devez sortir de la grotte avant de vous déplacer."); continue; }
            profondeur -= 50; if (profondeur < 0) profondeur = 0;
            printf("Vous remontez. Profondeur: -%dm.\n", profondeur);
        } else if (ch == 4) {
            inv_menu(&joueur);
        } else if (ch == 5) {
            if (en_grotte) {
                en_grotte = 0;
                puts("Vous sortez de la grotte.");
            } else {
                if (profondeur == 0) { puts("Il n'y a pas de grotte à la surface."); }
                else { en_grotte = 1; puts("Vous entrez dans une grotte (zone sûre)."); }
            }
        } else if (ch == 6) {
            if (!peut_sauver(profondeur, en_grotte)) {
                puts("Sauvegarde refusée : autorisée uniquement en surface ou dans une grotte.");
                continue;
            }
            if (!save_game("saves/slot1.save", &joueur, profondeur, en_grotte)) {
                puts("Échec de la sauvegarde.");
            }
        } else if (ch == 7) {
            if (!load_game("saves/slot1.save", &joueur, &profondeur, &en_grotte)) {
                puts("Échec du chargement.");
            }
        } else {
            puts("Au revoir.");
            break;
        }
    }
    return 0;
}
