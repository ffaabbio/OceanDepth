//
// Created by priss on 04/11/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <common.h>
#include <inventory.h>
#include <skills.h>

typedef struct Plongeur {
    int pv_max, pv;
    int o2_max, o2;
    int fatigue;      // 0..5
    int perles;       // monnaie
    int base_atk_min, base_atk_max;
    int base_defense;
    int skill_cd[SK_COUNT];
    Inventaire inv;
} Plongeur;

// Init joueur
void joueur_init(Plongeur* p);

// Stats effectives (avec equipements)
int  joueur_atk_min(const Plongeur* p);
int  joueur_atk_max(const Plongeur* p);
int  joueur_defense_totale(const Plongeur* p);
int  joueur_o2_cout_action(const Plongeur* p, int profondeur);
int  joueur_o2_cout_passif(const Plongeur* p, int profondeur);

// Tirage de degats (avec harpon si equipe)
int  joueur_degats_random(const Plongeur* p);

#endif
