#include "../include/map.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"

void displayMap(const Map *map, Player *player) {
    printf("CARTOGRAPHIE OCEANIQUE - SECTEUR %s\n\n", map->secteur);

    for (int i = 0; i < map->size; i++) {
        for (int j = 0; j < map->size; j++) {
            printf(" %s |", map->zones[i][j].name);
        }
        printf("\n");
    }

    printf("\nPosition actuelle: %s (%dm)", map->zones[player->x][player->y].name, player->depth);
}

void askAction(Map *map, Player *player) {
    printf("1-Se deplacer 2-Explorer la zone\n");
}

Zone generatePassiveZone() {
    Zone zone;
    int x = rand() % 5;

    switch (x) {
        case 0:
            strcpy(zone.name, "Debris");
            break;
        case 1:
            strcpy(zone.name, "Vide");
            return zone;
        case 2:
            strcpy(zone.name, "Algues");
            break;
        case 3:
            strcpy(zone.name, "Grotte");
            break;
        case 4:
            strcpy(zone.name, "Recif");
            break;
    }

    x = rand() % 11;

    // SPAWN PETIT ENNEMIE
    if (x >= 6) {

    }
}

Zone generateDangerZone(int depths) {
    Zone zone;
    Creature creature;

    int x = rand() % 5;

    switch (x) {
        case 0:
            zone.creatures[1] = spawnCreature("Requin", 80, 10, 20, 25, 15, "", 1, depths);

            return zone;
        case 1:
            zone.creatures[1] = spawnCreature("Requin Blanc", 145, 12, 18, 25, 15, "", 1, depths);

            return zone;
        case 2:
            strcpy(zone.name, "Anguilles");

            zone.creatures[1] = spawnCreature("Anguilles", 25, 12, 18, 5, 15, "", 1, depths);
            zone.creatures[2] = spawnCreature("Anguilles", 25, 12, 18, 5, 15, "", 1, depths);
            zone.creatures[3] = spawnCreature("Anguilles", 25, 12, 18, 5, 15, "", 1, depths);

            return zone;
        case 3:
            strcpy(zone.name, "Kraken");
            break;
        case 4:
            strcpy(zone.name, "Meduse Bleu");
            break;
    }

    x = rand() % 11;

    // SPAWN PETIT ENNEMIE
    if (x >= 6) {

    }
}

void generateZone(int level, Map *map) {
    for (int i = 0; i < map->size; i++) {
        if (strcmp(map->zones[level-1][i].name, "Inconnu") == 0) {
            int x = rand() % 11;

            // SAFE
            if (x < 6) {
                map->zones[level-1][i] = generatePassiveZone();
            }

            // ENNEMY
            else if (x < 9) {
                map->zones[level-1][i] = generateDangerZone(level * 50);
            }

            // BONUS
            else {

            }
        }
    }
}

void init(Map *map) {
    map->size = 4;
    strcpy(map->secteur, "PACIFIQUE");

    map->zones = malloc(sizeof(*map->zones) * map->size);

    for (int i = 0; i < map->size; i++) {
        map->zones[i] = malloc(sizeof(**map->zones) * map->size);
    }

    strcpy(map->zones[0][0].name, "Base");
    strcpy(map->zones[0][1].name, "Ocean");
    strcpy(map->zones[0][2].name, "Ocean");
    strcpy(map->zones[0][3].name, "Bateau");
    strcpy(map->zones[1][0].name, "Inconnu");
    strcpy(map->zones[1][1].name, "Inconnu");
    strcpy(map->zones[1][2].name, "Inconnu");
    strcpy(map->zones[1][3].name, "Inconnu");
    strcpy(map->zones[2][0].name, "Inconnu");
    strcpy(map->zones[2][1].name, "Inconnu");
    strcpy(map->zones[2][2].name, "Inconnu");
    strcpy(map->zones[2][3].name, "Inconnu");
    strcpy(map->zones[3][0].name, "Inconnu");
    strcpy(map->zones[3][1].name, "Inconnu");
    strcpy(map->zones[3][2].name, "Inconnu");
    strcpy(map->zones[3][3].name, "Inconnu");
}

