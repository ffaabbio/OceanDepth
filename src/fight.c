#include <stdio.h>
#include "../include/fight.h"

void startCombat(Player *player, Creature *creature) {
    printf("Combat started! %s vs Player.\n", creature->name);
    // For now: placeholder
    printf("Player attacks %s!\n", creature->name);
}
