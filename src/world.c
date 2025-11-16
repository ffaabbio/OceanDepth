#include <world.h>
#include <player.h>     // pour acceder a la combi et aux couts O2
#include <inventory.h>  // noms combi

static ZoneType pick_zone_for_row(int row){
    int r = rand_between(1,100);
    if (row==0) return Z_SURFACE; // surface fixe
    if (row==1){ // ~ -50 m
        if (r<=45) return Z_RECIF;
        else if (r<=75) return Z_ALGUES;
        else if (r<=90) return Z_EPAVE;
        else return Z_GROTTE;
    }
    if (row==2){ // ~ -150 m
        if (r<=35) return Z_ALGUES;
        else if (r<=55) return Z_EPAVE;
        else if (r<=75) return Z_GROTTE;
        else return Z_FOSSE;
    }
    // row==3  ~ -300 m
    if (r<=15) return Z_ALGUES;
    else if (r<=35) return Z_EPAVE;
    else if (r<=55) return Z_GROTTE;
    else return Z_FOSSE;
}

static int depth_for_row(int row){
    switch(row){
        case 0: return 0;
        case 1: return 50;
        case 2: return 150;
        default: return 300;
    }
}

void world_init(World* w){
    w->w = WORLD_W; w->h = WORLD_H;
    for(int y=0;y<w->h;y++){
        for(int x=0;x<w->w;x++){
            w->cells[y][x].type = pick_zone_for_row(y);
            w->cells[y][x].profondeur = depth_for_row(y);
            w->cells[y][x].visited = 0;
        }
    }
    // Ligne surface = 0 -> force SURFACE partout
    for(int x=0;x<w->w;x++){
        w->cells[0][x].type = Z_SURFACE;
        w->cells[0][x].profondeur = 0;
    }
    // spawn joueur : surface, colonne centrale
    w->px = w->w/2; w->py = 0;
    w->cells[w->py][w->px].visited = 1;
}

const Zone* world_current_zone(const World* w){
    return &w->cells[w->py][w->px];
}

int world_can_enter(const Plongeur* j, const Zone* z){
    // exigences combi : -150 -> Composite, -300 -> Titanium
    if (z->profondeur >= 300) {
        return j->inv.eq_combi >= COMBI_TITANIUM;
    }
    if (z->profondeur >= 150) {
        return j->inv.eq_combi >= COMBI_COMPOSITE;
    }
    return 1;
}

static char zone_char(ZoneType t){
    switch(t){
        case Z_SURFACE: return 'S';
        case Z_RECIF:   return 'R';
        case Z_ALGUES:  return 'A';
        case Z_EPAVE:   return 'E';
        case Z_GROTTE:  return 'G';
        case Z_FOSSE:   return 'F';
        default:        return '?';
    }
}

void world_print(const World* w, const Plongeur* j){
    puts("");
    puts("============== Carte OceanDepth ==============");
    puts("");

    // En-tete des colonnes
    printf("          ");
    for (int x = 0; x < w->w; ++x) {
        printf("  %d   ", x);
    }
    puts("");
    puts("        +-----+-----+-----+-----+-----+");

    for (int y = 0; y < w->h; ++y) {
        int depth = depth_for_row(y);

        printf("%4dm   |", -depth);

        for (int x = 0; x < w->w; ++x) {
            int isP = (x == w->px && y == w->py);
            char c  = zone_char(w->cells[y][x].type);
            printf("  %c  |", isP ? 'P' : c);
        }
        puts("");
        puts("        +-----+-----+-----+-----+-----+");
    }

    puts("");
    puts("Legende des zones :");
    puts("  S = Surface     R = Recifs      A = Algues");
    puts("  E = Epave       G = Grotte      F = Fosse");
    puts("  P = Position du joueur");

    // Infos combi
    int db, od;
    combi_stats(j->inv.eq_combi, &db, &od);
    printf("\nCombi equipee: %s  (DEF +%d, O2/tour %d)\n",
           nom_combi(j->inv.eq_combi), db, od);

    puts("Regles d'acces :");
    puts("  -150m et plus profond => Combi Composite minimale");
    puts("  -300m                 => Combi Titanium obligatoire");

    // infos sur la case actuelle
    const Zone* z = world_current_zone(w);
    char zc = zone_char(z->type);
    const char* zname = "?";
    switch (z->type) {
        case Z_SURFACE: zname = "Surface"; break;
        case Z_RECIF:   zname = "Recifs"; break;
        case Z_ALGUES:  zname = "Algues"; break;
        case Z_EPAVE:   zname = "Epave"; break;
        case Z_GROTTE:  zname = "Grotte"; break;
        case Z_FOSSE:   zname = "Fosse"; break;
        default:        zname = "Inconnu"; break;
    }

    printf("\nPosition actuelle : profondeur -%dm, zone %c (%s)\n",
           z->profondeur, zc, zname);
}



static void apply_o2_move_and_check(Plongeur* j, int profondeur){
    j->o2 -= joueur_o2_cout_passif(j, profondeur); // cout de deplacement base sur profondeur d'arrivee
    if (j->o2 < 0) j->o2 = 0;
    if (j->o2 == 0){
        puts("[CRITIQUE] Oxygene epuise lors du deplacement ! Vous suffoquez (-5 PV).");
        j->pv -= 5;
        if (j->pv < 0) j->pv = 0;
    }
}

int world_move(World* w, Plongeur* j, int dx, int dy, int* nouvelle_profondeur){
    int nx = w->px + dx;
    int ny = w->py + dy;
    if (nx < 0 || nx >= w->w || ny < 0 || ny >= w->h){
        puts("Impossible: bord de la carte.");
        return 0;
    }
    const Zone* target = &w->cells[ny][nx];
    if (!world_can_enter(j, target)){
        puts("Acces refuse: combinaison insuffisante pour cette profondeur.");
        return 0;
    }
    // Appliquer le cout O2 du deplacement (profondeur d'arrivee)
    apply_o2_move_and_check(j, target->profondeur);
    if (j->pv <= 0){
        // on a suffoque jusqu'a la mort; ne pas deplacer
        return 0;
    }
    w->px = nx; w->py = ny;
    w->cells[ny][nx].visited = 1;
    if (nouvelle_profondeur) *nouvelle_profondeur = target->profondeur;
    printf("Vous vous deplacez en -%dm, zone %c.\n", target->profondeur, zone_char(target->type));
    return 1;
}
