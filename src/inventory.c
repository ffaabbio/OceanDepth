#include <stdio.h>
#include "../include/inventory.h"

void displayInventory(const Inventory *inv) {
    printf("Inventaire (%d objet(s)):\n", inv->itemCount);
    for (int i = 0; i < inv->itemCount; i++) {
        printf(" - %s (x%d)\n", inv->items[i].name, inv->items[i].quantity);
    }
    printf("\n");
}
