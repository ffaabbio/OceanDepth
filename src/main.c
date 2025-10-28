#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "map.h"
#include "../include/player.h"
#include "../include/creature.h"
#include "../include/fight.h"
#include "../include/inventory.h"

int main() {
    /*typedef struct {
        int id;
        char name[30];
        int maxHealthPoints;
        int currentHealthPoints;
        int minAttack;
        int maxAttack;
        int defense;
        int speed;
        char specialEffect[20]; // "paralysis", "poison", "none"
        int isAlive;
    } Creature;*/

    Player player;
    player.inventory.itemCount = 0;
    player.x = 0;
    player.y = 0;
    player.depth = 0;
    for (int i = 0; i < MAX_ITEMS; i++) {
        player.inventory.items[i].quantity = 0;
        player.inventory.items[i].name[0] = '\0'; // chaîne vide
    }

    Creature * creature = malloc(sizeof(Creature));


    //Initialisation des parametres par defaut du joueur
    player.maxHealthPoints = 100;
    player.healthPoints = player.maxHealthPoints;
    player.maxOxygenLevel = 50;
    player.oxygenLevel = player.maxOxygenLevel;
    player.attack = 10;
    player.fatigueLevel = 1;
    player.maxFatigueLevel = 5;
    player.pearls = 0;
    player.inventory.itemCount = 2;
    player.inventory.items[0].quantity = 1;
    strcpy(player.inventory.items[0].name,"Couteau en plastique");
    player.inventory.items[0].attackImp = 2;
    player.inventory.items[1].quantity = 1;
    strcpy(player.inventory.items[1].name,"Capsule d'oxygene");
    player.inventory.items[1].attackImp = 0;

    //initialisation par defaut du monstre
    strcpy(creature->name, "Requin");
    creature->maxHealthPoints = 80;
    creature->currentHealthPoints = creature->maxHealthPoints;
    creature->minAttack =  10;
    creature->maxAttack = 20;
    creature->isAlive = 1;

    printf("=== OceanDepths ===\n\n");

    startCombat(&player, creature);

    free(creature);

    return 0;
}
