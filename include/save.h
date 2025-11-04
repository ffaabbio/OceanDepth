//
// Created by priss on 04/11/2025.
//

#ifndef SAVE_H
#define SAVE_H

#include <common.h>

/* Forward pour éviter les inclusions circulaires */
typedef struct Plongeur Plongeur;

/* Sauver/Charger au format texte OCEANDEPTHS_SAVE_V1.
   Retourne 1 en cas de succès, 0 sinon. */
int save_game(const char* path, const Plongeur* j, int profondeur, int en_grotte);
int load_game(const char* path, Plongeur* j, int* profondeur, int* en_grotte);

#endif
