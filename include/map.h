//
// Created by Hdido on 28/10/2025.
//

#ifndef OCEANDEPTHS_MAP_H
#define OCEANDEPTHS_MAP_H
#include "creature.h"
#include "inventory.h"
#include "player.h"

typedef struct {
    char name[30 * sizeof(char)];
    Creature creatures[3 * sizeof(Creature)];
    Item loots[1 * sizeof(Item)];
} Zone;

typedef struct {
    char secteur[30];
    int size;
    Zone **zones;
} Map;

void displayMap(const Map *map, Player *player);

void init(Map *map);

#endif //OCEANDEPTHS_MAP_H