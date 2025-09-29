#include <stdio.h>
#include <stdlib.h>
#include "../include/fight.h"

int playerAttack(Player *p, Creature *c) {
    int damages = p->attack * p->inventory.items[0].attackImp;
    c->currentHealthPoints -= damages;
    return damages;
}

int creatureAttack(Player *p, Creature *c) {
    int damages = rand() % (c->maxAttack - c->minAttack + 1) + c->minAttack;
    p->healthPoints -= damages;
    return damages;
}

void startCombat(Player *p, Creature *creature) {
    printf("Que le combat commence! %s vs Joueur !\n\n", creature->name);
    int lap = 1;
    int damages = 0;
    int action = 1;
    while (p->healthPoints > 0 && creature->currentHealthPoints >0 && action != 0) {

        displayPlayer(p);
        displayInventory(&p->inventory);
        displayCreature(creature);
        printf("Tour %d\n",lap/2);

        if (lap%2 != 0) {
            printf("Veuillez choisir parmis les actions ci-dessous:\n0 - Fuir\n1 - Attaquer\n");
            scanf("%d", &action);
            if (action == 1) {
                damages = playerAttack(p, creature);
                printf("%d Degats recus !\n\n",damages);
                lap++;
            }
        }
        else {
            damages = creatureAttack(p, creature);
            printf("%d Degats recus !\n\n",damages);
            lap++;
        }

    }

    if (p->healthPoints <= 0) {
        p->healthPoints = 0;
        printf("Le %s sauvage vous a vaincu ...\n\n DEFAITE",creature->name);
    }
    else if (creature->currentHealthPoints <= 0) {
        creature->currentHealthPoints = 0;
        printf("Vous etes venu a bout du %s sauvage !!\n\n VICTOIRE",creature->name);
    }

    else {
        printf("Vous avez fui le combat #boufon\n");
    }
}
