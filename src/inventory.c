#include <inventory.h>
#include <player.h>
#include <creature.h>


// ----- Noms -----
const char* nom_consommable(int t){
    switch((ConsommableType)t){
        case CONS_O2: return "Capsule O2";
        case CONS_SOIN: return "Trousse Soin";
        case CONS_STIM: return "Stimulant";
        case CONS_ANTIDOTE: return "Antidote";
        default: return "Inconnu";
    }
}
const char* nom_harpon(int h){
    switch((HarponType)h){
        case HARPON_ROUILLE: return "Harpon Rouille";
        case HARPON_TRIDENT: return "Trident";
        case HARPON_ELECTRIQUE: return "Harpon Electrique";
        case HARPON_LASER: return "Harpon Laser";
        default: return "Aucun";
    }
}
const char* nom_combi(int c){
    switch((CombiType)c){
        case COMBI_NEOPRENE: return "Neoprene Basic";
        case COMBI_COMPOSITE: return "Composite";
        case COMBI_TITANIUM: return "Armure Titanium";
        default: return "Aucune";
    }
}

// ----- Helpers -----
static int inv_find_same(Item slots[], ItemKind k, int sub){
    for(int i=0;i<INV_MAX;i++){
        if (slots[i].kind==k && slots[i].subtype==sub) return i;
    }
    return -1;
}
static int inv_find_empty(Item slots[]){
    for(int i=0;i<INV_MAX;i++) if (slots[i].kind==0) return i;
    return -1;
}

// ----- Ajout -----
int inv_add_consumable(Inventaire* inv, ConsommableType t, int qty){
    int idx = inv_find_same(inv->slots, ITEM_CONS, t);
    if (idx<0) idx = inv_find_empty(inv->slots);
    if (idx<0) return 0;
    inv->slots[idx].kind=ITEM_CONS; inv->slots[idx].subtype=t; inv->slots[idx].qty += qty;
    return 1;
}
int inv_add_harpon(Inventaire* inv, HarponType h){
    if (inv_find_same(inv->slots, ITEM_HARPON, h)>=0) return 1;
    int idx = inv_find_empty(inv->slots);
    if (idx<0) return 0;
    inv->slots[idx].kind=ITEM_HARPON; inv->slots[idx].subtype=h; inv->slots[idx].qty=1;
    return 1;
}
int inv_add_combi(Inventaire* inv, CombiType c){
    if (inv_find_same(inv->slots, ITEM_COMBI, c)>=0) return 1;
    int idx = inv_find_empty(inv->slots);
    if (idx<0) return 0;
    inv->slots[idx].kind=ITEM_COMBI; inv->slots[idx].subtype=c; inv->slots[idx].qty=1;
    return 1;
}

// ----- Affichage inventaire -----
void inv_print(const Inventaire* inv, int perles){
    puts("\n                    INVENTAIRE SOUS-MARIN");
    puts("┌─────────────────┬─────────────────┬─────────────────┐");
    printf("│  ÉQUIPÉ: HARPON │   ÉQUIPÉ: COMBI │    PERLES: %-4d  │\n", perles);
    printf("│ %-15s │ %-15s │                 │\n", nom_harpon(inv->eq_harpon), nom_combi(inv->eq_combi));
    int db,od; combi_stats(inv->eq_combi,&db,&od);
    int mi,ma,oc,di; harpon_stats(inv->eq_harpon,&mi,&ma,&oc,&di);
    printf("│ ATK:%2d-%-2d      │ DEF:+%-2d        │ O2 atk:%-2d/tour:%-2d │\n",
           mi>0?mi:0, ma>0?ma:0, db, oc, od);
    puts("└─────────────────┴─────────────────┴─────────────────┘");
    for (int i=0;i<INV_MAX;i++){
        const Item* it=&inv->slots[i];
        if (i%3==0) puts("├─────────────────┼─────────────────┼─────────────────┤");
        if (it->kind==ITEM_CONS) {
            printf("│ [%d] %-11s │ qty:%-7d    ", i+1, nom_consommable(it->subtype), it->qty);
        } else if (it->kind==ITEM_HARPON) {
            printf("│ [%d] %-11s │ (harpon)   ", i+1, nom_harpon(it->subtype));
        } else if (it->kind==ITEM_COMBI) {
            printf("│ [%d] %-11s │ (combi)    ", i+1, nom_combi(it->subtype));
        } else {
            printf("│ [%d] %-11s │ %-11s", i+1, "Vide", "");
        }
        if (i%3==2) puts("│"); else printf("│ ");
    }
    puts("\n1-Utiliser objet  2-Équiper harpon  3-Équiper combi  4-Retour");
}

// ----- Utilisation consommable -----
static int appliquer_consommable(Plongeur* j, ConsommableType t){
    switch (t){
        case CONS_O2:   j->o2 = clampi(j->o2 + 40, 0, j->o2_max); puts("+40 Oxygène."); return 1;
        case CONS_SOIN: j->pv = clampi(j->pv + 25, 0, j->pv_max); puts("+25 PV."); return 1;
        case CONS_STIM: j->fatigue = clampi(j->fatigue - 2, 0, 5); puts("Fatigue -2."); return 1;
        case CONS_ANTIDOTE: puts("Antidote utilisé."); return 1; // simplifié
        default: return 0;
    }
}

static int inv_use_from_index(Plongeur* j, int idx){
    if (idx<0 || idx>=INV_MAX) return 0;
    Item* it = &j->inv.slots[idx];
    if (it->kind!=ITEM_CONS || it->qty<=0) return 0;
    if (appliquer_consommable(j, (ConsommableType)it->subtype)) {
        it->qty--; if (it->qty<=0){ it->kind=0; it->subtype=0; it->qty=0; }
        return 1;
    }
    return 0;
}

// Menu rapide en combat (montre seulement les consommables)
int inv_use_menu_combat(Plongeur* j){
    int map[INV_MAX]; int n=0;
    puts("Consommables disponibles :");
    for(int i=0;i<INV_MAX;i++){
        if (j->inv.slots[i].kind==ITEM_CONS && j->inv.slots[i].qty>0){
            printf("  %d) %s x%d\n", n+1, nom_consommable(j->inv.slots[i].subtype), j->inv.slots[i].qty);
            map[n]=i; n++;
        }
    }
    if (n==0){ puts("  (aucun)"); return 0; }
    int choix = lire_entier_borne("> ", 1, n);
    return inv_use_from_index(j, map[choix-1]);
}

// Grand menu (hors combat)
void inv_menu(Plongeur* j){
    for(;;){
        inv_print(&j->inv, j->perles);
        int choix = lire_entier_borne("> ", 1, 4);
        if (choix==1){
            // Utiliser objet
            puts("Choisir un slot consommable (1..8) :");
            int idx = lire_entier_borne("> ", 1, INV_MAX);
            inv_use_from_index(j, idx-1);
        } else if (choix==2){
            // Équiper harpon
            int has=0; int map[INV_MAX]; int n=0;
            puts("Harpons possédés :");
            for(int i=0;i<INV_MAX;i++) if (j->inv.slots[i].kind==ITEM_HARPON){
                    printf("  %d) %s\n", n+1, nom_harpon(j->inv.slots[i].subtype));
                    map[n]=j->inv.slots[i].subtype; n++; has=1;
                }
            if (!has){ puts("  (aucun)"); continue; }
            int ch = lire_entier_borne("> ", 1, n);
            j->inv.eq_harpon = (HarponType)map[ch-1];
            puts("Harpon équipé.");
        } else if (choix==3){
            // Équiper combi
            int has=0; int map[INV_MAX]; int n=0;
            puts("Combis possédées :");
            for(int i=0;i<INV_MAX;i++) if (j->inv.slots[i].kind==ITEM_COMBI){
                    printf("  %d) %s\n", n+1, nom_combi(j->inv.slots[i].subtype));
                    map[n]=j->inv.slots[i].subtype; n++; has=1;
                }
            if (!has){ puts("  (aucune)"); continue; }
            int ch = lire_entier_borne("> ", 1, n);
            j->inv.eq_combi = (CombiType)map[ch-1];
            puts("Combinaison équipée.");
        } else {
            break;
        }
    }
}

// ----- Récompenses après victoire -----
void distribuer_recompenses(Plongeur* j, struct CreatureMarine* ennemis, int nb, int profondeur){
    int perles = 0;
    for (int i=0;i<nb;i++){
        // Perles communes
        perles += rand_between(5,15);
        // Bonus "forte créature"
        // (simplifié : si Kraken/Requin -> +rare)
        // (tu peux raffiner par profondeur)
    }
    j->perles += perles;
    printf("Vous gagnez %d perles (total: %d).\n", perles, j->perles);

    // Chance d’objets
    if (rand_between(1,100) <= 50) inv_add_consumable(&j->inv, CONS_O2, 1);
    if (rand_between(1,100) <= 40) inv_add_consumable(&j->inv, CONS_SOIN, 1);
    if (rand_between(1,100) <= 25) inv_add_consumable(&j->inv, CONS_STIM, 1);

    // Petit taux pour un équipement
    int roll = rand_between(1,100);
    if (roll <= 10) {
        // Harpon rare selon profondeur
        if (profondeur >= 200) inv_add_harpon(&j->inv, HARPON_ELECTRIQUE);
        else inv_add_harpon(&j->inv, HARPON_TRIDENT);
        puts("[TRÉSOR] Vous trouvez un harpon !");
    } else if (roll <= 18) {
        if (profondeur >= 200) inv_add_combi(&j->inv, COMBI_TITANIUM);
        else inv_add_combi(&j->inv, COMBI_COMPOSITE);
        puts("[TRÉSOR] Vous trouvez une combinaison !");
    }
}
