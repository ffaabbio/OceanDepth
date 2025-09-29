#include <stdio.h>
#include "../include/player.h"

void displayPlayer(const Player *p) {
    printf("Player - HP: %d/%d | O2: %d/%d | Fatigue: %d | Pearls: %d\n",
           p->healthPoints, p->maxHealthPoints,
           p->oxygenLevel, p->maxOxygenLevel,
           p->fatigueLevel, p->pearls);
}