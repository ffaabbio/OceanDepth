#ifndef PLAYER_H
#define PLAYER_H
#include"inventory.h"

typedef struct {
    char * name;
    int x;
    int y;
    int depth;
    int healthPoints;
    int maxHealthPoints;
    int oxygenLevel;
    int maxOxygenLevel;
    int attack;
    int fatigueLevel; // entre 0 et 5
    int maxFatigueLevel;
    int pearls;
    Inventory inventory;
} Player;

void displayPlayer(const Player *p);

#endif