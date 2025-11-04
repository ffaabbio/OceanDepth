#include <fight.h>
#include <skills.h>
#include <string.h>

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
    print_bar("Oxygene", j->o2, j->o2_max, 30);
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

static void trier_par_vitesse(CreatureMarine arr[], int n, const int speed_delta[]) {
    // tri bulle simple en tenant compte du malus de vitesse éventuel
    for (int i = 0; i < n; ++i)
        for (int j = 0; j + 1 < n; ++j) {
            int vj  = arr[j].vitesse   + (speed_delta ? speed_delta[arr[j].id-1] : 0);
            int vj1 = arr[j+1].vitesse + (speed_delta ? speed_delta[arr[j+1].id-1] : 0);
            if (vj < vj1) { CreatureMarine t=arr[j]; arr[j]=arr[j+1]; arr[j+1]=t; }
        }
}

/* ==== AJOUT COMPETENCES ==== */
// états temporaires par combat
typedef struct {
    int pacify_turns[MAX_CREATURES];    // 1 si pacifié (saute 1 action)
    int speed_delta[MAX_CREATURES];     // -2 sur 1 tour via Tourbillon
} EffetsCombat;

static void effets_clear(EffetsCombat* ef){
    memset(ef, 0, sizeof(*ef));
}

static void effets_fin_tour(EffetsCombat* ef){
    for (int i=0;i<MAX_CREATURES;i++){
        if (ef->pacify_turns[i] > 0) ef->pacify_turns[i]--;
        if (ef->speed_delta[i]  != 0) ef->speed_delta[i] = 0; // tourbillon dure 1 tour
    }
}

static void show_skills(const Plongeur* j, int profondeur){
    puts("Competences disponibles :");
    for (int id=0; id<SK_COUNT; ++id){
        int cost, cd; skill_cost_cd(j, (SkillId)id, profondeur, &cost, &cd);
        printf("  %d) %-21s  (cout O2:%d, CD:%d, restant:%d)\n",
               id+1, skill_name((SkillId)id), cost, cd, j->skill_cd[id]);
    }
}

static int use_skill(Plongeur* j, int profondeur, EffetsCombat* eff, CreatureMarine ennemis[], int nb){
    show_skills(j, profondeur);
    int choix = lire_entier_borne("> ", 1, SK_COUNT);
    SkillId id = (SkillId)(choix - 1);

    if (j->skill_cd[id] > 0) { puts("Cette competence est en cooldown."); return 0; }

    int cost, cd; skill_cost_cd(j, id, profondeur, &cost, &cd);
    if (j->o2 < cost) { puts("Oxygene insuffisant pour cette competence."); return 0; }

    switch (id) {
        case SK_APNEE: {
            j->o2 = clampi(j->o2 + 20, 0, j->o2_max);
            puts("[Apnee Prolongee] +20 O2.");
            j->skill_cd[id] = cd;
        } break;
        case SK_DECHARGE: {
            int total = 0, cibles = 0;
            for (int i=0;i<nb;i++){
                if (!ennemis[i].est_vivant) continue;
                int d = rand_between(20,30);
                total += appliquer_degats_creature(&ennemis[i], d); cibles++;
            }
            printf("[Decharge Electrique] %d cibles touchees, total ~%d degats.\n", cibles, total);
            j->o2 = clampi(j->o2 - cost, 0, j->o2_max);
            j->skill_cd[id] = cd;
        } break;
        case SK_COMMUNICATION: {
            // choisir cible vivante
            int ids[MAX_CREATURES], nids=0;
            for (int i=0;i<nb;i++) if (ennemis[i].est_vivant) ids[nids++] = ennemis[i].id;
            if (nids == 0) { puts("Aucune cible."); return 0; }
            printf("Cible a pacifier ? "); for (int i=0;i<nids;i++) printf("%d%s", ids[i], (i+1==nids)?"":" ");
            printf("\n");
            int cid = lire_entier_borne("> ", 1, MAX_CREATURES);
            // appliquer 1 tour de pacif
            eff->pacify_turns[cid-1] = 1;
            printf("[Communication Marine] La creature #%d sera pacifiee 1 tour.\n", cid);
            j->o2 = clampi(j->o2 - cost, 0, j->o2_max);
            j->skill_cd[id] = cd;
        } break;
        case SK_TOURBILLON: {
            for (int i=0;i<nb;i++) if (ennemis[i].est_vivant) eff->speed_delta[ennemis[i].id-1] = -2;
            puts("[Tourbillon] -2 Vitesse pour toutes les creatures jusqu'au prochain tour.");
            j->o2 = clampi(j->o2 - cost, 0, j->o2_max);
            j->skill_cd[id] = cd;
        } break;
        default: return 0;
    }
    return 1;
}
/* ==== FIN AJOUT COMPETENCES ==== */

IssueCombat lancer_combat(Plongeur* j, CreatureMarine bestiaire[MAX_CREATURES], int nb, int profondeur) {
    int paralysie_prochain_tour = 0;
    EffetsCombat effets; effets_clear(&effets);

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
            printf("\nActions : 1-Attaquer  2-Objets  3-Competences  4-Fin du tour\n");
            int choix = lire_entier_borne("> ", 1, 4);

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

                int base = joueur_degats_random(j);
                if (cible->type == TYPE_POISSON_EPEE) base += 2;

                int reels = appliquer_degats_creature(cible, base);
                printf("Vous harponnez %s ! Degats infliges: %d\n", cible->nom, reels);

                j->o2 -= joueur_o2_cout_action(j, profondeur);
                if (j->o2 < 0) j->o2 = 0;
                j->fatigue = clampi(j->fatigue + 1, 0, 5);
                actions--;

            } else if (choix == 2) {
                (void)inv_use_menu_combat(j);
            } else if (choix == 3) {
                if (use_skill(j, profondeur, &effets, bestiaire, nb)) {
                    // une competence consomme 1 action si elle a un effet (même Apnée)
                    actions--;
                }
            } else {
                fin_tour = 1;
            }

            if (j->o2 == 0) {
                puts("[CRITIQUE] Oxygene epuise ! Vous suffoquez (-5 PV).");
                j->pv -= 5;
                if (j->pv <= 0) return ISSUE_DEFAITE;
            }
        }

        // O2 passif + recup fatigue
        j->o2 -= joueur_o2_cout_passif(j, profondeur);
        if (j->o2 < 0) j->o2 = 0;
        if (j->o2 == 0) {
            puts("[CRITIQUE] Oxygene epuise ! Vous suffoquez (-5 PV).");
            j->pv -= 5;
            if (j->pv <= 0) return ISSUE_DEFAITE;
        }
        if (j->fatigue > 0) j->fatigue--;

        // tick cooldowns des competences
        for (int i=0;i<SK_COUNT;i++) if (j->skill_cd[i] > 0) j->skill_cd[i]--;

        // Tour des creatures (ordre vitesse avec malus tourbillon)
        CreatureMarine ordre[MAX_CREATURES]; int k=0;
        for (int i=0;i<nb;i++) if (bestiaire[i].est_vivant) ordre[k++]=bestiaire[i];
        trier_par_vitesse(ordre,k, effets.speed_delta);

        for (int idx=0; idx<k; ++idx){
            CreatureMarine* c=NULL;
            for (int i=0;i<nb;i++) if (bestiaire[i].est_vivant && bestiaire[i].id==ordre[idx].id){ c=&bestiaire[i]; break; }
            if (!c) continue;

            // Pacifie ? (communication marine)
            if (effets.pacify_turns[c->id-1] > 0) {
                printf("%s est pacifie et n'attaque pas ce tour.\n", c->nom);
                continue;
            }

            int deg = rand_between(c->attaque_minimale, c->attaque_maximale) - joueur_defense_totale(j);
            if (deg < 1) deg = 1;
            if (c->type==TYPE_REQUIN && c->points_de_vie_actuels*2 < c->points_de_vie_max){
                deg += (deg*30)/100; puts("Requin en frenesie !");
            }
            printf("%s vous attaque. Degats subis: %d\n", c->nom, deg);
            j->pv -= deg;
            if (j->o2 > 0) j->o2 = clampi(j->o2 - 1, 0, j->o2_max);
            if (j->pv <= 0) return ISSUE_DEFAITE;

            if (c->type==TYPE_MEDUSE){
                if (rand_between(1,100) <= 25){
                    puts(">> Piqure paralysante ! -1 action au prochain tour.");
                    // applique sur le joueur (meme mecanique qu'avant)
                    // NOTE: on conserve le flag existant
                    // on ne le stocke pas ici, on utilise la variable paralysie_prochain_tour existante :
                    paralysie_prochain_tour = 1;
                }
            }
            if (c->type==TYPE_KRAKEN){
                int d2 = rand_between(c->attaque_minimale, c->attaque_maximale) - joueur_defense_totale(j);
                if (d2 < 1) d2 = 1;
                printf("%s enchaine ! Degats: %d\n", c->nom, d2);
                j->pv -= d2;
                if (j->o2 > 0) j->o2 = clampi(j->o2 - 1, 0, j->o2_max);
                if (j->pv <= 0) return ISSUE_DEFAITE;
            }
        }

        // fin de tour : effacer/decroitre effets temporaires d'1 tour
        effets_fin_tour(&effets);
    }
    return ISSUE_DEFAITE;
}
