#include <player.h>

void joueur_init(Plongeur* p) {
    p->pv_max = 100; p->pv = p->pv_max;
    p->o2_max = 100; p->o2 = p->o2_max;
    p->fatigue = 0;
    p->perles = 0;
    p->base_atk_min = 16; p->base_atk_max = 26;
    p->base_defense = 3;

    // Inventaire & equipements de depart
    for (int i=0;i<INV_MAX;i++){ p->inv.slots[i].kind=0; p->inv.slots[i].subtype=0; p->inv.slots[i].qty=0; }
    p->inv.eq_harpon = HARPON_ROUILLE;
    p->inv.eq_combi  = COMBI_NEOPRENE;

    // Donne 1 capsule O2 et 1 trousse
    inv_add_consumable(&p->inv, CONS_O2,   1);
    inv_add_consumable(&p->inv, CONS_SOIN, 1);

    for (int i=0;i<SK_COUNT;i++) p->skill_cd[i] = 0;
}

// ----- Equipements -----
void harpon_stats(HarponType h, int* amin, int* amax, int* o2c, int* def_ign) {
    int mi=0, ma=0, oc=0, di=0;
    switch (h) {
        case HARPON_ROUILLE:    mi=12; ma=18; oc=2; di=0; break;  // sujet: basique
        case HARPON_TRIDENT:    mi=18; ma=28; oc=3; di=1; break;
        case HARPON_ELECTRIQUE: mi=28; ma=42; oc=4; di=3; break;  // ignore 3 DEF
        case HARPON_LASER:      mi=35; ma=50; oc=5; di=4; break;
        default:                mi=0;  ma=0;  oc=0; di=0; break;
    }
    if (amin) *amin=mi;
    if (amax) *amax=ma;
    if (o2c) *o2c=oc;
    if (def_ign) *def_ign=di;
}

void combi_stats(CombiType c, int* defb, int* o2delta) {
    int db=0, od=0;
    switch (c) {
        case COMBI_NEOPRENE:  db=5;  od=-1; break; // -1 O2/tour (reduite la conso)
        case COMBI_COMPOSITE: db=12; od=-1; break;
        case COMBI_TITANIUM:  db=25; od=-2; break;
        default:              db=0;  od=0;  break;
    }
    if (defb) *defb=db;
    if (o2delta) *o2delta=od;
}

// ----- Stats effectives -----
int joueur_atk_min(const Plongeur* p){
    int mi,ma,oc,di; harpon_stats(p->inv.eq_harpon,&mi,&ma,&oc,&di);
    return (mi>0?mi:p->base_atk_min);
}
int joueur_atk_max(const Plongeur* p){
    int mi,ma,oc,di; harpon_stats(p->inv.eq_harpon,&mi,&ma,&oc,&di);
    return (ma>0?ma:p->base_atk_max);
}
int joueur_defense_totale(const Plongeur* p){
    int db,od; combi_stats(p->inv.eq_combi,&db,&od);
    return p->base_defense + db;
}
int joueur_o2_cout_action(const Plongeur* p, int profondeur){
    int base;
    if (profondeur < 100) base=2;
    else if (profondeur < 200) base=3;
    else if (profondeur < 300) base=4;
    else base=5;
    int mi,ma,oc,di; harpon_stats(p->inv.eq_harpon,&mi,&ma,&oc,&di);
    return base + oc; // coût = profondeur + arme
}
int joueur_o2_cout_passif(const Plongeur* p, int profondeur){
    int base;
    if (profondeur < 100) base=2;
    else if (profondeur < 200) base=3;
    else if (profondeur < 300) base=4;
    else base=5;
    int db,od; combi_stats(p->inv.eq_combi,&db,&od);
    int val = base + od; // od <= 0 (réduction)
    if (val < 1) val = 1;
    return val;
}

int joueur_degats_random(const Plongeur* p) {
    int mi = joueur_atk_min(p);
    int ma = joueur_atk_max(p);
    if (ma < mi) ma = mi;
    return rand_between(mi, ma);
}
