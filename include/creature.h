#ifndef CREATURE_H
#define CREATURE_H

//Loot ?
typedef struct {
    char name[30];
    int maxHealthPoints;
    int currentHealthPoints;
    int minAttack;
    int maxAttack;
    int defense;
    int speed;
    char specialEffect[20]; // "paralysis", "poison", "none"
    int isAlive;
} Creature;

void displayCreature(const Creature *c);

Creature spawnCreature(char* name, int maxHealthPoints, int minAttack, int maxAttack, int defense, int speed, char* specialEffect, int isAlive, int depth);

#endif
