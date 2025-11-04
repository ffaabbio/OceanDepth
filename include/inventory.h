//
// Created by priss on 04/11/2025.
//

#ifndef INVENTORY_H
#define INVENTORY_H

#include <common.h>

/* Forward typedefs pour éviter les conflits de types */
typedef struct Plongeur Plongeur;
typedef struct CreatureMarine CreatureMarine;


// ----- Types d'objets -----
typedef enum { ITEM_CONS=1, ITEM_HARPON, ITEM_COMBI } ItemKind;

typedef enum { CONS_O2=1, CONS_SOIN, CONS_STIM, CONS_ANTIDOTE } ConsommableType;

typedef enum {
    HARPON_NONE=0, HARPON_ROUILLE, HARPON_TRIDENT, HARPON_ELECTRIQUE, HARPON_LASER
} HarponType;

typedef enum {
    COMBI_NONE=0, COMBI_NEOPRENE, COMBI_COMPOSITE, COMBI_TITANIUM
} CombiType;

typedef struct {
    ItemKind kind;
    int      subtype;   // ConsommableType | HarponType | CombiType
    int      qty;       // quantité (1 pour les équipements)
} Item;

#define INV_MAX 8

typedef struct {
    Item slots[INV_MAX];
    HarponType eq_harpon;
    CombiType  eq_combi;
} Inventaire;

// ----- Noms lisibles -----
const char* nom_consommable(int sub);
const char* nom_harpon(int sub);
const char* nom_combi(int sub);

// ----- Statistiques d'équipement (absolues) -----
void harpon_stats(HarponType h, int* atk_min, int* atk_max, int* o2_cost, int* def_ignore);
void combi_stats(CombiType c, int* def_bonus, int* o2_passive_delta);

// ----- Gestion d'inventaire -----
int  inv_add_consumable(Inventaire* inv, ConsommableType t, int qty);
int  inv_add_harpon(Inventaire* inv, HarponType h);
int  inv_add_combi(Inventaire* inv, CombiType c);
void inv_print(const Inventaire* inv, int perles);

int  inv_use_menu_combat(Plongeur* j);        // menu rapide pendant combat (consommables)
void inv_menu(Plongeur* j);                   // grand menu hors combat

// ----- Récompenses -----
void distribuer_recompenses(Plongeur* j,
                            CreatureMarine* ennemis, int nb, int profondeur);
#endif

