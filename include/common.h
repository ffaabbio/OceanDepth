//
// Created by priss on 04/11/2025.
//

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_CREATURES 4
#define NOM_TAILLE 30

static inline int clampi(int v, int lo, int hi){ return v<lo?lo:(v>hi?hi:v); }
static inline int rand_between(int a,int b){ if(b<a){int t=a;a=b;b=t;} return (rand()%(b-a+1))+a; }

static inline int lire_entier_borne(const char* invite,int min,int max){
    char buf[128];
    for(;;){
        if(invite && *invite) fputs(invite, stdout);
        if(!fgets(buf,sizeof buf,stdin)){ clearerr(stdin); continue; }
        char *end; long v=strtol(buf,&end,10);
        while(end && *end && isspace((unsigned char)*end)) end++;
        if(end && *end=='\0' && v>=min && v<=max) return (int)v;
        printf("Entree invalide. Saisir un nombre entre %d et %d.\n",min,max);
    }
}
#endif
