#include <fight.h>

static int conso_o2_action(int profondeur) {
    if (profondeur < 100) return 2;
    if (profondeur < 200) return 3;
    if (profondeur < 300) return 4;
    return 5;
}

static int conso_o2_passive(int profondeur) {
    return conso_o2_action(profondeur);
}

static int max_attaques_selon_fatigue(int f) {
    if (f <= 1) return 3;
    if (f <= 3) return 2;
    return 1;
}

static void print_bar(const char* label, int val, int max, int largeur) {
    int filled = (max > 0) ? (val * largeur) / max : 0;
    if (filled < 0) filled = 0;
    if (filled > largeur) filled = largeur;
    printf("%-9s [", label);
    for (int i = 0; i < largeur; ++i) putchar(i < filled ? '#' : '.');
    printf("] %d/%d\n", val, max);
}

static void afficher_etat(const Plongeur* j, const CreatureMarine* c, int nb, int profondeur) {
    printf("\n=== OceanDepth — Profondeur: -%dm ===\n", profondeur);
    print_bar("Vie", j->pv, j->pv_max, 40);
    print_bar("Oxygène", j->o2, j->o2_max, 30);
    printf("Fatigue   [%d/5]\n", j->fatigue);
    puts("-----------------------------------------------");
    for (int i = 0; i < nb; ++i) {
        if (!c[i].est_vivant) continue;
        printf("  [%d] %-14s  PV:%3d/%-3d  DEF:%d  VIT:%d  Effet:%s\n",
               c[i].id, c[i].nom, c[i].points_de_vie_actuels, c[i].points_de_vie_max,
               c[i].defense, c[i].vitesse, c[i].effet_special);
    }
    puts("-----------------------------------------------");
}

static int toutes_mortes(const CreatureMarine* c, int nb) {
    for (int i = 0; i < nb; ++i) if (c[i].est_vivant) return 0;
    return 1;
}

// dégâts appliqués à une créature (min 1), avec effets spéciaux défensifs
static int appliquer_degats_creature(CreatureMarine* c, int degats_bruts) {
    int def = c->defense;
    int deg = degats_bruts - def;
    if (c->type == TYPE_CRABE) {
        // carapace: -20% des dégâts restants
        int red = (deg * 20) / 100;
        deg -= red;
    }
    if (deg < 1) deg = 1;
    c->points_de_vie_actuels -= deg;
    if (c->points_de_vie_actuels <= 0) {
        c->points_de_vie_actuels = 0;
        c->est_vivant = 0;
    }
    return deg;
}

// ordre d'attaque approximé par vitesse (tri bulle simple sur vue locale)
static void trier_par_vitesse(CreatureMarine arr[], int n) {
    int m = n;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j + 1 < m; ++j) {
            if (arr[j].vitesse < arr[j+1].vitesse) {
                CreatureMarine tmp = arr[j]; arr[j] = arr[j+1]; arr[j+1] = tmp;
            }
        }
    }
}

IssueCombat lancer_combat(Plongeur* j, CreatureMarine bestiaire[MAX_CREATURES], int nb, int profondeur) {
    // copie locale triable pour l'ordre de vitesse des ennemis
    CreatureMarine ordre[MAX_CREATURES];
    int k = 0;
    for (int i = 0; i < nb; ++i) if (bestiaire[i].est_vivant) ordre[k++] = bestiaire[i];

    int paralysie_prochain_tour = 0;

    while (j->pv > 0) {
        if (toutes_mortes(bestiaire, nb)) return ISSUE_VICTOIRE;

        afficher_etat(j, bestiaire, nb, profondeur);

        int attaques_restantes = max_attaques_selon_fatigue(j->fatigue);
        if (paralysie_prochain_tour) {
            if (attaques_restantes > 0) attaques_restantes--;
            paralysie_prochain_tour = 0;
            printf("[Effet] Paralysie : -1 action ce tour.\n");
        }

        int tour_fini = 0;
        while (!tour_fini && attaques_restantes > 0 && j->pv > 0) {
            printf("\nActions : 1-Attaquer  2-Objets  3-Fin du tour\n");
            int choix = lire_entier_borne("> ", 1, 3);

            if (choix == 1) {
                // choisir cible
                int ids[MAX_CREATURES], nids = 0;
                for (int i = 0; i < nb; ++i) if (bestiaire[i].est_vivant) ids[nids++] = bestiaire[i].id;
                if (nids == 0) break;
                printf("Cible ? ");
                for (int i = 0; i < nids; ++i) printf("%d%s", ids[i], (i+1==nids)?"":" ");
                printf("\n");
                int cible_id = lire_entier_borne("> ", 1, MAX_CREATURES);

                CreatureMarine* cible = NULL;
                for (int i = 0; i < nb; ++i) {
                    if (bestiaire[i].id == cible_id && bestiaire[i].est_vivant) { cible = &bestiaire[i]; break; }
                }
                if (!cible) { printf("Cible invalide.\n"); continue; }

                int base = joueur_degats_random(j);
                // effet Poisson-Épée : ignore 2 DEF (coté attaquant, on booste juste le brut)
                if (cible->type == TYPE_POISSON_EPEE) base += 2;

                int reels = appliquer_degats_creature(cible, base);
                printf("Vous harponnez %s ! Dégâts infligés: %d\n", cible->nom, reels);

                j->o2 -= conso_o2_action(profondeur);
                if (j->o2 < 0) j->o2 = 0;
                j->fatigue = clampi(j->fatigue + 1, 0, 5);
                attaques_restantes--;

            } else if (choix == 2) {
                printf("Objets: [1] Capsule O2 (+40) x%d, [2] Trousse (+25 PV) x%d, [3] Retour\n",
                       j->inv.capsules_o2, j->inv.trousses_soin);
                int co = lire_entier_borne("> ", 1, 3);
                if (co == 1) {
                    if (j->inv.capsules_o2 > 0) {
                        j->inv.capsules_o2--;
                        j->o2 = clampi(j->o2 + 40, 0, j->o2_max);
                        printf("+40 Oxygène.\n");
                    } else puts("Aucune capsule disponible.");
                } else if (co == 2) {
                    if (j->inv.trousses_soin > 0) {
                        j->inv.trousses_soin--;
                        j->pv = clampi(j->pv + 25, 0, j->pv_max);
                        printf("+25 PV.\n");
                    } else puts("Aucune trousse disponible.");
                } else {
                    // retour
                }
            } else {
                tour_fini = 1;
            }

            if (j->o2 == 0) {
                puts("[CRITIQUE] Oxygène épuisé ! Vous suffoquez (-5 PV).");
                j->pv -= 5;
                if (j->pv <= 0) return ISSUE_DEFAITE;
            }
            if (attaques_restantes == 0) tour_fini = 1;
        }

        // conso passive + récup fatigue
        j->o2 -= conso_o2_passive(profondeur);
        if (j->o2 < 0) j->o2 = 0;
        if (j->o2 == 0) {
            puts("[CRITIQUE] Oxygène épuisé ! Vous suffoquez (-5 PV).");
            j->pv -= 5;
            if (j->pv <= 0) return ISSUE_DEFAITE;
        }
        if (j->fatigue > 0) j->fatigue--;

        // --- Tour des créatures (ordre vitesse) ---
        // rafraîchir la liste 'ordre' depuis bestiaire
        k = 0; for (int i = 0; i < nb; ++i) if (bestiaire[i].est_vivant) ordre[k++] = bestiaire[i];
        trier_par_vitesse(ordre, k);

        for (int idx = 0; idx < k; ++idx) {
            // retrouver la référence réelle (pour garder PV à jour)
            CreatureMarine* c = NULL;
            for (int i = 0; i < nb; ++i)
                if (bestiaire[i].est_vivant && bestiaire[i].id == ordre[idx].id) { c = &bestiaire[i]; break; }
            if (!c) continue;

            int deg = rand_between(c->attaque_minimale, c->attaque_maximale) - j->defense;
            if (deg < 1) deg = 1;

            if (c->type == TYPE_REQUIN && c->points_de_vie_actuels * 2 < c->points_de_vie_max) {
                deg += (deg * 30) / 100; // frénésie
                printf("%s entre en frénésie !\n", c->nom);
            }

            printf("%s vous attaque. Dégâts subis: %d\n", c->nom, deg);
            j->pv -= deg;
            if (j->o2 > 0) j->o2 = clampi(j->o2 - 1, 0, j->o2_max); // stress
            if (j->pv <= 0) return ISSUE_DEFAITE;

            if (c->type == TYPE_MEDUSE) {
                if (rand_between(1,100) <= 25) {
                    paralysie_prochain_tour = 1;
                    printf(">> Piqûre paralysante ! -1 action au prochain tour.\n");
                }
            }

            if (c->type == TYPE_KRAKEN) {
                int deg2 = rand_between(c->attaque_minimale, c->attaque_maximale) - j->defense;
                if (deg2 < 1) deg2 = 1;
                printf("%s enchaîne une seconde attaque ! Dégâts: %d\n", c->nom, deg2);
                j->pv -= deg2;
                if (j->o2 > 0) j->o2 = clampi(j->o2 - 1, 0, j->o2_max);
                if (j->pv <= 0) return ISSUE_DEFAITE;
            }
        }
    }
    return ISSUE_DEFAITE;
}
