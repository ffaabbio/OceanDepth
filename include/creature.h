//
// Created by priss on 04/11/2025.
//

#ifndef CREATURE_H
#define CREATURE_H

#include <common.h>

typedef enum {
    TYPE_KRAKEN = 1,
    TYPE_REQUIN,
    TYPE_MEDUSE,
    TYPE_POISSON_EPEE,
    TYPE_CRABE
} TypeCreature;

/* IMPORTANT : struct TAGGÉE pour matcher le forward 'typedef struct CreatureMarine CreatureMarine;' */
typedef struct CreatureMarine {
    int id;                       // 1..MAX_CREATURES
    TypeCreature type;
    char nom[NOM_TAILLE];

    int points_de_vie_max;
    int points_de_vie_actuels;
    int attaque_minimale;
    int attaque_maximale;
    int defense;
    int vitesse;
    char effet_special[20];
    int est_vivant;
} CreatureMarine;

int generer_creatures(CreatureMarine out[MAX_CREATURES], int profondeur);
const char* creature_type_nom(TypeCreature t);

#endif

