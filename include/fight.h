//
// Created by priss on 04/11/2025.
//

#ifndef FIGHT_H
#define FIGHT_H

#include <player.h>
#include <creature.h>

typedef enum { ISSUE_CONTINUE=0, ISSUE_VICTOIRE=1, ISSUE_DEFAITE=2 } IssueCombat;
IssueCombat lancer_combat(Plongeur* j, CreatureMarine bestiaire[MAX_CREATURES], int nb, int profondeur);
#endif


