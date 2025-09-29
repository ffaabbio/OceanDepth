#ifndef INVENTORY_H
#define INVENTORY_H

#define MAX_ITEMS 8

typedef struct {
    char name[30];
    int attackImp;
    int quantity;
} Item;

typedef struct {
    Item items[MAX_ITEMS];
    int itemCount;
} Inventory;

void displayInventory(const Inventory *inv);

#endif
