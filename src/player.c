#include <player.h>

void joueur_init(Plongeur* p) {
    p->pv_max = 100;   p->pv = p->pv_max;
    p->o2_max = 100;   p->o2 = p->o2_max;
    p->fatigue = 0;
    p->perles = 0;
    p->atk_min = 16;   p->atk_max = 26;
    p->defense = 3;
    p->inv.capsules_o2 = 1;
    p->inv.trousses_soin = 1;
    p->inv.antidotes = 0;
    p->inv.stimulants = 0;
}

int joueur_degats_random(const Plongeur* p) {
    return rand_between(p->atk_min, p->atk_max);
}
