#include <stdio.h>
#include "../include/player.h"
#include <stdlib.h>

void displayPlayer(const Player *p) {
    printf("Joueur - Vie: %d/%d | Oxygene: %d/%d | Attaque: %d | Fatigue: %d | Perles: %d\n\n",
           p->healthPoints, p->maxHealthPoints,
           p->oxygenLevel, p->maxOxygenLevel,
           p->attack,
           p->fatigueLevel, p->pearls);
}