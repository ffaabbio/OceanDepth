#include <stdio.h>
#include "../include/creature.h"

#include <string.h>

void displayCreature(const Creature *c) {
    printf("[%s] - Vie: %d/%d | Attaque: %d-%d | En vie: %s\n\n",
           c->name,
           c->currentHealthPoints, c->maxHealthPoints,
           c->minAttack, c->maxAttack,
           c->isAlive ? "Oui" : "Non");
}

Creature spawnCreature(char* name, int maxHealthPoints, int minAttack, int maxAttack, int defense, int speed, char* specialEffect, int isAlive, int depth) {
    Creature creature;

    strcpy(creature.name, name);
    creature.maxHealthPoints = maxHealthPoints * (depth/100);
    creature.currentHealthPoints = maxHealthPoints * (depth/100);
    creature.minAttack = minAttack * (depth/100);
    creature.maxAttack = maxAttack * (depth/100);
    creature.defense = defense * (depth/100);
    creature.speed = speed * (depth/100);
    strcpy(creature.specialEffect, specialEffect);
    creature.isAlive = isAlive;

    return creature;
}
