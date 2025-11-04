#include <fight.h>

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

static int appliquer_degats_creature(CreatureMarine* c, int degats_bruts) {
    int def = c->defense;
    int deg = degats_bruts - def;
    if (c->type == TYPE_CRABE) {
        int red = (deg * 20) / 100;
        deg -= red;
    }
    if (deg < 1) deg = 1;
    c->points_de_vie_actuels -= deg;
    if (c->points_de_vie_actuels <= 0) { c->points_de_vie_actuels = 0; c->est_vivant = 0; }
    return deg;
}

static void trier_par_vitesse(CreatureMarine arr[], int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j + 1 < n; ++j)
            if (arr[j].vitesse < arr[j+1].vitesse) { CreatureMarine t=arr[j]; arr[j]=arr[j+1]; arr[j+1]=t; }
}

IssueCombat lancer_combat(Plongeur* j, CreatureMarine bestiaire[MAX_CREATURES], int nb, int profondeur) {
    int paralysie_prochain_tour = 0;

    while (j->pv > 0) {
        if (toutes_mortes(bestiaire, nb)) return ISSUE_VICTOIRE;
        afficher_etat(j, bestiaire, nb, profondeur);

        int max_actions;
        if (j->fatigue <= 1) max_actions = 3;
        else if (j->fatigue <= 3) max_actions = 2;
        else max_actions = 1;

        if (paralysie_prochain_tour && max_actions > 0) {
            max_actions--; paralysie_prochain_tour = 0;
            puts("[Effet] Paralysie : -1 action ce tour.");
        }

        int actions = max_actions;
        int fin_tour = 0;
        while (!fin_tour && actions > 0 && j->pv > 0) {
            printf("\nActions : 1-Attaquer  2-Objets  3-Fin du tour\n");
            int choix = lire_entier_borne("> ", 1, 3);

            if (choix == 1) {
                int ids[MAX_CREATURES], nids = 0;
                for (int i = 0; i < nb; ++i) if (bestiaire[i].est_vivant) ids[nids++] = bestiaire[i].id;
                if (nids == 0) break;
                printf("Cible ? "); for (int i = 0; i < nids; ++i) printf("%d%s", ids[i], (i+1==nids)?"":" ");
                printf("\n");
                int cible_id = lire_entier_borne("> ", 1, MAX_CREATURES);

                CreatureMarine* cible = NULL;
                for (int i = 0; i < nb; ++i) if (bestiaire[i].id == cible_id && bestiaire[i].est_vivant){ cible=&bestiaire[i]; break; }
                if (!cible){ puts("Cible invalide."); continue; }

                int base = joueur_degats_random(j); // 🆕 dégâts avec harpon
                // Effets ennemis (Poisson-Épée ignore 2 DEF côté receveur → on booste le brut un peu)
                if (cible->type == TYPE_POISSON_EPEE) base += 2;

                int reels = appliquer_degats_creature(cible, base);
                printf("Vous harponnez %s ! Dégâts infligés: %d\n", cible->nom, reels);

                // O2 action (équipements pris en compte) 🆕
                j->o2 -= joueur_o2_cout_action(j, profondeur);
                if (j->o2 < 0) j->o2 = 0;
                j->fatigue = clampi(j->fatigue + 1, 0, 5);
                actions--;

            } else if (choix == 2) {
                // Menu rapide consommables 🆕
                (void)inv_use_menu_combat(j);
            } else {
                fin_tour = 1;
            }

            if (j->o2 == 0) {
                puts("[CRITIQUE] Oxygène épuisé ! Vous suffoquez (-5 PV).");
                j->pv -= 5;
                if (j->pv <= 0) return ISSUE_DEFAITE;
            }
        }

        // O2 passif (avec combi) + récup fatigue 🆕
        j->o2 -= joueur_o2_cout_passif(j, profondeur);
        if (j->o2 < 0) j->o2 = 0;
        if (j->o2 == 0) {
            puts("[CRITIQUE] Oxygène épuisé ! Vous suffoquez (-5 PV).");
            j->pv -= 5;
            if (j->pv <= 0) return ISSUE_DEFAITE;
        }
        if (j->fatigue > 0) j->fatigue--;

        // Tour des créatures (ordre vitesse)
        CreatureMarine ordre[MAX_CREATURES]; int k=0;
        for (int i=0;i<nb;i++) if (bestiaire[i].est_vivant) ordre[k++]=bestiaire[i];
        trier_par_vitesse(ordre,k);
        for (int idx=0; idx<k; ++idx){
            CreatureMarine* c=NULL;
            for (int i=0;i<nb;i++) if (bestiaire[i].est_vivant && bestiaire[i].id==ordre[idx].id){ c=&bestiaire[i]; break; }
            if (!c) continue;

            int deg = rand_between(c->attaque_minimale, c->attaque_maximale) - joueur_defense_totale(j); // 🆕 DEF avec combi
            if (deg < 1) deg = 1;
            if (c->type==TYPE_REQUIN && c->points_de_vie_actuels*2 < c->points_de_vie_max){
                deg += (deg*30)/100; puts("Requin en frénésie !");
            }
            printf("%s vous attaque. Dégâts subis: %d\n", c->nom, deg);
            j->pv -= deg;
            if (j->o2 > 0) j->o2 = clampi(j->o2 - 1, 0, j->o2_max); // stress
            if (j->pv <= 0) return ISSUE_DEFAITE;

            if (c->type==TYPE_MEDUSE){
                if (rand_between(1,100) <= 25){ paralysie_prochain_tour = 1; puts(">> Piqûre paralysante ! -1 action au prochain tour."); }
            }
            if (c->type==TYPE_KRAKEN){
                int d2 = rand_between(c->attaque_minimale, c->attaque_maximale) - joueur_defense_totale(j);
                if (d2 < 1) d2 = 1;
                printf("%s enchaîne ! Dégâts: %d\n", c->nom, d2);
                j->pv -= d2;
                if (j->o2 > 0) j->o2 = clampi(j->o2 - 1, 0, j->o2_max);
                if (j->pv <= 0) return ISSUE_DEFAITE;
            }
        }
    }
    return ISSUE_DEFAITE;
}
