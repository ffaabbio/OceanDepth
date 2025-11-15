//
// Created by priss on 04/11/2025.
//



#include <creature.h>

const char* creature_type_nom(TypeCreature t){
    switch(t){
        case TYPE_KRAKEN: return "Kraken";
        case TYPE_REQUIN: return "Requin-Tigre";
        case TYPE_MEDUSE: return "Meduse Bleue";
        case TYPE_POISSON_EPEE: return "Poisson-Epee";
        case TYPE_CRABE:  return "Crabe Geant";
        default: return "Inconnu";
    }
}

static void remplir_creature(CreatureMarine* c, TypeCreature t, int id, int profondeur){
    memset(c,0,sizeof *c);
    c->id = id; c->est_vivant = 1; c->type = t;
    strncpy(c->nom, creature_type_nom(t), NOM_TAILLE-1);
    strcpy(c->effet_special, "aucun");

    switch(t){
        case TYPE_KRAKEN:        // 120-180 PV, 25-40 ATK
            c->points_de_vie_max = rand_between(120,180);
            c->attaque_minimale = 25; c->attaque_maximale = 40;
            c->defense = 4 + (profondeur>=200?2:0);
            c->vitesse = 10;
            break;
        case TYPE_REQUIN:        // 60-100 PV, 15-25 ATK (rapide)
            c->points_de_vie_max = rand_between(60,100);
            c->attaque_minimale = 15; c->attaque_maximale = 25;
            c->defense = 2 + (profondeur>=150?1:0);
            c->vitesse = 20;
            break;
        case TYPE_MEDUSE:        // 20-40 PV, 8-15 ATK (paralysie)
            c->points_de_vie_max = rand_between(20,40);
            c->attaque_minimale = 8;  c->attaque_maximale = 15;
            c->defense = 0; c->vitesse = 15;
            strcpy(c->effet_special, "paralysie");
            break;
        case TYPE_POISSON_EPEE:  // 70-90 PV, 18-28 ATK
            c->points_de_vie_max = rand_between(70,90);
            c->attaque_minimale = 18; c->attaque_maximale = 28;
            c->defense = 2; c->vitesse = 12;
            break;
        case TYPE_CRABE:         // 80-120 PV, 12-20 ATK (haute DEF)
            c->points_de_vie_max = rand_between(80,120);
            c->attaque_minimale = 12; c->attaque_maximale = 20;
            c->defense = 6; c->vitesse = 8;
            break;
    }
    c->points_de_vie_actuels = c->points_de_vie_max;
}

int generer_creatures(CreatureMarine out[MAX_CREATURES], int profondeur){
    int nb = rand_between(1, MAX_CREATURES);
    for(int i=0;i<nb;i++){
        // Pondération simple suivant profondeur (plus profond => plus dur)
        int r = rand_between(1,100);
        TypeCreature t;
        if (profondeur < 100) {
            if (r<=35) t=TYPE_REQUIN;
            else if (r<=65) t=TYPE_POISSON_EPEE;
            else if (r<=85) t=TYPE_MEDUSE;
            else t=TYPE_CRABE;
        } else if (profondeur < 200) {
            if (r<=30) t=TYPE_REQUIN;
            else if (r<=55) t=TYPE_POISSON_EPEE;
            else if (r<=80) t=TYPE_CRABE;
            else t=TYPE_MEDUSE;
        } else {
            if (r<=20) t=TYPE_KRAKEN;
            else if (r<=50) t=TYPE_REQUIN;
            else if (r<=70) t=TYPE_CRABE;
            else if (r<=85) t=TYPE_POISSON_EPEE;
            else t=TYPE_MEDUSE;
        }
        remplir_creature(&out[i], t, i+1, profondeur);
    }
    return nb;
}
