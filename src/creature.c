#include <stdio.h>
#include "../include/creature.h"

void displayCreature(const Creature *c) {
    printf("[%s] - Vie: %d/%d | Attaque: %d-%d | En vie: %s\n\n",
           c->name,
           c->currentHealthPoints, c->maxHealthPoints,
           c->minAttack, c->maxAttack,
           c->isAlive ? "Oui" : "Non");
}
