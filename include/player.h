//
// Created by priss on 04/11/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <common.h>
#include <inventory.h>

typedef struct {
    int pv_max, pv;
    int o2_max, o2;
    int fatigue;      // 0..5
    int perles;
    int atk_min, atk_max;
    int defense;
    Inventaire inv;
} Plongeur;

void joueur_init(Plongeur* p);
int  joueur_degats_random(const Plongeur* p);

#endif

