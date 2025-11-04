#include <stdio.h>

#include <creature.h>

static void header(int profondeur){
    printf("\nOceanDepths - Générateur de créatures  (profondeur: -%dm)\n", profondeur);
    puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

int main(void){
    srand((unsigned)time(NULL));
    int profondeur = lire_entier_borne("Choisir profondeur (0..300): ", 0, 300);

    CreatureMarine tab[MAX_CREATURES] = {0};
    int nb = generer_creatures(tab, profondeur);
    header(profondeur);

    for(int i=0;i<nb;i++){
        CreatureMarine* c = &tab[i];
        printf("[%d] %-14s  PV:%3d/%-3d  ATK:%2d-%-2d  DEF:%d  VIT:%d  Effet:%s\n",
               c->id, c->nom, c->points_de_vie_actuels, c->points_de_vie_max,
               c->attaque_minimale, c->attaque_maximale, c->defense, c->vitesse, c->effet_special);
    }
    puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    return 0;
}

