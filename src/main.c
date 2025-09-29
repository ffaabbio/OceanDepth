#include <stdio.h>
#include "../include/player.h"
#include "../include/creature.h"
#include "../include/fight.h"
#include "../include/inventory.h"

int main() {
    Player player = {100, 100, 80, 100, 0, 10};
    Creature creature = {1, "Shark", 90, 90, 15, 25, 5, 7, "none", 1};
    Inventory inv = {{{"Oxygen Capsule", 2}, {"Medkit", 1}}, 2};

    printf("=== OceanDepths ===\n\n");

    displayPlayer(&player);
    displayCreature(&creature);
    displayInventory(&inv);

    startCombat(&player, &creature);

    return 0;
}
