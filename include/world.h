//
// Created by priss on 04/11/2025.
//

#ifndef WORLD_H
#define WORLD_H

#include <common.h>

/* Forward pour éviter les cycles */
typedef struct Plongeur Plongeur;

#define WORLD_W 5
#define WORLD_H 4

typedef enum {
    Z_SURFACE = 0,
    Z_RECIF,
    Z_ALGUES,
    Z_EPAVE,
    Z_GROTTE,
    Z_FOSSE
} ZoneType;

typedef struct {
    ZoneType type;
    int profondeur;   // 0, 50, 150, 300
    int visited;      // bool
} Zone;

typedef struct {
    int w, h;
    int px, py;             // position joueur (col, ligne)
    Zone cells[WORLD_H][WORLD_W];
} World;

void world_init(World* w);                                  // génère la carte et place le joueur à la surface
void world_print(const World* w, const Plongeur* j);        // affiche carte + position + infos combi
const Zone* world_current_zone(const World* w);             // renvoie la zone courante
int  world_move(World* w, Plongeur* j, int dx, int dy, int* nouvelle_profondeur); // déplacement + coût O2
int  world_can_enter(const Plongeur* j, const Zone* z);     // vérifie l’équipement pour la profondeur

#endif

