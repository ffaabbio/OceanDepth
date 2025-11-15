#include <fight.h>
#include <inventory.h>
#include <save.h>
#include <world.h>

static void banner(void) {
    puts("==============================================");
    puts("      OceanDepth - v0.5 (carte/exploration)   ");
    puts("==============================================");
}

static int peut_sauver(int profondeur, int en_grotte) {
    return (profondeur == 0) || en_grotte;
}

int main(void) {
    srand((unsigned)time(NULL));
    banner();

    Plongeur joueur; joueur_init(&joueur);

    World world; world_init(&world);
    int profondeur = world_current_zone(&world)->profondeur;
    int en_grotte  = 0;

    for (;;) {
        printf("\nMenu : 1-Carte/Se deplacer  2-Explorer  3-Inventaire  4-Grotte Entrer/Sortir  5-Sauver  6-Charger  7-Quitter\n");
        int ch = lire_entier_borne("> ", 1, 7);

        if (ch == 1) {
            if (en_grotte) { puts("Vous etes dans une grotte. Sortez avant de vous deplacer."); continue; }
            world_print(&world, &joueur);
            puts("Deplacements : Z=Haut, S=Bas, Q=Gauche, D=Droite, 0=Retour au menu.");
            for (;;) {
                char buf[16]={0};
                if (!fgets(buf, sizeof buf, stdin)) { clearerr(stdin); break; }
                if (buf[0]=='0' || buf[0]=='\n') break;
                int dx=0, dy=0;
                if (buf[0]=='z' || buf[0]=='Z') dy=-1;
                else if (buf[0]=='s' || buf[0]=='S') dy=+1;
                else if (buf[0]=='q' || buf[0]=='Q') dx=-1;
                else if (buf[0]=='d' || buf[0]=='D') dx=+1;
                else { puts("Touche invalide. Z/S/Q/D ou 0 pour quitter."); continue; }
                if (world_move(&world, &joueur, dx, dy, &profondeur)){
                    // ok; afficher mini etat
                    printf("Etat: PV=%d/%d  O2=%d/%d  Fatigue=%d  Perles=%d\n",
                           joueur.pv, joueur.pv_max, joueur.o2, joueur.o2_max, joueur.fatigue, joueur.perles);
                }
                if (joueur.pv <= 0) { puts("Vous avez succombe... Fin de partie."); return 0; }
            }

        } else if (ch == 2) {
            const Zone* z = world_current_zone(&world);
            if (en_grotte || z->type == Z_GROTTE) {
                puts("Zone sure (grotte) : rien a explorer ici.");
                continue;
            }
            if (z->type == Z_EPAVE) {
                // 60% tresor, 40% combat court
                int r = rand_between(1,100);
                if (r <= 60){
                    int bonus = rand_between(10,25);
                    joueur.perles += bonus;
                    puts("[Epave] Vous trouvez un tresor !");
                    printf("+%d perles (total: %d).\n", bonus, joueur.perles);
                    if (rand_between(1,100) <= 50) inv_add_consumable(&joueur.inv, CONS_SOIN, 1);
                    if (rand_between(1,100) <= 35) inv_add_consumable(&joueur.inv, CONS_O2, 1);
                    if (rand_between(1,100) <= 15) inv_add_harpon(&joueur.inv, HARPON_TRIDENT);
                } else {
                    CreatureMarine ennemis[MAX_CREATURES]={0};
                    int nb = generer_creatures(ennemis, profondeur);
                    printf("\nDes creatures gardent l'epave... %d ennemi(s)!\n", nb);
                    IssueCombat res = lancer_combat(&joueur, ennemis, nb, profondeur);
                    if (res == ISSUE_VICTOIRE) distribuer_recompenses(&joueur, ennemis, nb, profondeur);
                    else { puts("Vous avez succombe..."); break; }
                }
            } else {
                // exploration classique -> combat
                CreatureMarine ennemis[MAX_CREATURES]={0};
                int nb = generer_creatures(ennemis, profondeur);
                printf("\nVous explorez... %d creature(s) approchent !\n", nb);
                IssueCombat res = lancer_combat(&joueur, ennemis, nb, profondeur);
                if (res == ISSUE_VICTOIRE) distribuer_recompenses(&joueur, ennemis, nb, profondeur);
                else { puts("Vous avez succombe..."); break; }
            }

        } else if (ch == 3) {
            inv_menu(&joueur);

        } else if (ch == 4) {
            const Zone* z = world_current_zone(&world);
            if (z->type != Z_GROTTE) {
                puts("Aucune grotte ici. Deplacez-vous vers une case 'G'.");
                continue;
            }
            en_grotte = !en_grotte;
            if (en_grotte) puts("Vous entrez dans la grotte (zone sure).");
            else puts("Vous sortez de la grotte.");

        } else if (ch == 5) {
            if (!peut_sauver(profondeur, en_grotte)) {
                puts("Sauvegarde refusee : autorisee uniquement en surface ou dans une grotte.");
                continue;
            }
            if (!save_game("saves/slot1.save", &joueur, profondeur, en_grotte)) {
                puts("Echec de la sauvegarde.");
            }

        } else if (ch == 6) {
            if (!load_game("saves/slot1.save", &joueur, &profondeur, &en_grotte)) {
                puts("Echec du chargement.");
            } else {
                // recaler la position sur la ligne correspondant a la profondeur, colonne centrale
                world.px = world.w/2;
                if (profondeur==0) world.py=0;
                else if (profondeur<=50) world.py=1;
                else if (profondeur<=150) world.py=2;
                else world.py=3;
                world.cells[world.py][world.px].visited = 1;
            }

        } else {
            puts("Au revoir.");
            break;
        }
    }
    return 0;
}
