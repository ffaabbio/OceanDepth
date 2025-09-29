#include <stdio.h>
#include "../include/creature.h"

void displayCreature(const Creature *c) {
    printf("Creature [%s] - HP: %d/%d | ATK: %d-%d | DEF: %d | Speed: %d | Alive: %s\n",
           c->name,
           c->currentHealthPoints, c->maxHealthPoints,
           c->minAttack, c->maxAttack,
           c->defense, c->speed,
           c->isAlive ? "yes" : "no");
}
