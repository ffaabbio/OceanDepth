//
// Created by priss on 04/11/2025.
//

#ifndef OCEANDEPTH_SKILLS_H
#define OCEANDEPTH_SKILLS_H

#endif //OCEANDEPTH_SKILLS_H

#ifndef SKILLS_H
#define SKILLS_H

#include <common.h>

/* Forward pour eviter les cycles */
typedef struct Plongeur Plongeur;

typedef enum {
    SK_APNEE = 0,
    SK_DECHARGE,
    SK_COMMUNICATION,
    SK_TOURBILLON,
    SK_COUNT
} SkillId;

const char* skill_name(SkillId id);

/* Donne cout O2 et CD de base de la competence (independants de la profondeur pour v1) */
static inline void skill_cost_cd(const Plongeur* j, SkillId id, int profondeur, int* o2_cost, int* cd_base) {
    (void)j; (void)profondeur;
    int c = 0, cd = 0;
    switch (id) {
        case SK_APNEE:        c = 0; cd = 2; break;
        case SK_DECHARGE:     c = 8; cd = 3; break;
        case SK_COMMUNICATION:c = 3; cd = 2; break;
        case SK_TOURBILLON:   c = 4; cd = 3; break;
        default:              c = 0; cd = 0; break;
    }
    if (o2_cost) *o2_cost = c;
    if (cd_base) *cd_base = cd;
}

#endif
