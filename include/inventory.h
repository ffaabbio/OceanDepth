#ifndef INVENTORY_H
#define INVENTORY_H

#define MAX_ITEMS 8

typedef struct {
    char name[30];
    int quantity;
} InventoryItem;

typedef struct {
    InventoryItem items[MAX_ITEMS];
    int itemCount;
} Inventory;

void displayInventory(const Inventory *inv);

#endif
