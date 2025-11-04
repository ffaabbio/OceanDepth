//
// Created by priss on 04/11/2025.
//

#ifndef CREATURE_H
#define CREATURE_H
#include <common.h>

typedef enum {
    TYPE_KRAKEN=1, TYPE_REQUIN, TYPE_MEDUSE, TYPE_POISSON_EPEE, TYPE_CRABE
} TypeCreature;

typedef struct {
    int id;                      // 1..MAX_CREATURES
    char nom[NOM_TAILLE];        // "Kraken", "Requin-Tigre", ...
    int points_de_vie_max;       // PV max
    int points_de_vie_actuels;   // PV
    int attaque_minimale;        // ATK min
    int attaque_maximale;        // ATK max
    int defense;                 // DEF
    int vitesse;                 // ordre
    char effet_special[20];      // "paralysie", "aucun", ...
    int est_vivant;              // bool
    TypeCreature type;           // interne pour la logique
} CreatureMarine;

int generer_creatures(CreatureMarine out[MAX_CREATURES], int profondeur);
const char* creature_type_nom(TypeCreature t);

#endif
