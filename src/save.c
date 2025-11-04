//
// Created by priss on 04/11/2025.
//

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <save.h>
#include <player.h>
#include <inventory.h>

#if defined(_WIN32)
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
  #define MKDIR(p) mkdir((p), 0700)
#endif

#define SAVE_MAGIC "OCEANDEPTHS_SAVE_V1"

static void ensure_saves_dir(void){
    FILE* f = fopen("saves/.touch", "rb");
    if (f) { fclose(f); return; }
    MKDIR("saves");
    f = fopen("saves/.touch", "wb");
    if (f) fclose(f);
}

int save_game(const char* path, const Plongeur* j, int profondeur, int en_grotte){
    if (!j || !path) return 0;
    ensure_saves_dir();

    FILE* f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "Erreur: impossible d'ouvrir '%s' en écriture : %s\n", path, strerror(errno));
        return 0;
    }

    /* Ligne 1 : entête */
    fprintf(f, "%s\n", SAVE_MAGIC);
    /* Ligne 2 : profondeur + grotte */
    fprintf(f, "PROFONDEUR %d GROTTE %d\n", profondeur, en_grotte ? 1 : 0);
    /* Ligne 3 : joueur (base stats) */
    fprintf(f, "JOUEUR %d %d %d %d %d %d %d %d %d\n",
            j->pv, j->pv_max, j->o2, j->o2_max, j->fatigue, j->perles,
            j->base_atk_min, j->base_atk_max, j->base_defense);
    /* Ligne 4 : équipements */
    fprintf(f, "EQUIP %d %d\n", j->inv.eq_harpon, j->inv.eq_combi);

    /* Ligne 5 : inventaire */
    int count = 0;
    for (int i=0;i<INV_MAX;i++) if (j->inv.slots[i].kind) count++;
    fprintf(f, "INV %d\n", count);
    for (int i=0;i<INV_MAX;i++){
        const Item* it = &j->inv.slots[i];
        if (!it->kind) continue;
        fprintf(f, "ITEM %d %d %d\n", it->kind, it->subtype, it->qty);
    }

    fclose(f);
    printf("Sauvegarde effectuée dans '%s'.\n", path);
    return 1;
}

int load_game(const char* path, Plongeur* j, int* profondeur, int* en_grotte){
    if (!j || !path || !profondeur || !en_grotte) return 0;

    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Erreur: impossible d'ouvrir '%s' en lecture : %s\n", path, strerror(errno));
        return 0;
    }

    char magic[64]={0};
    if (!fgets(magic, sizeof magic, f)) { fclose(f); return 0; }
    /* enlever \n éventuel */
    size_t L = strlen(magic); if (L && magic[L-1]=='\n') magic[L-1]=0;

    if (strcmp(magic, SAVE_MAGIC) != 0) {
        fprintf(stderr, "Fichier de sauvegarde invalide (entête inattendue).\n");
        fclose(f); return 0;
    }

    /* PROFONDEUR/GROTTE */
    int prof=0, grot=0;
    if (fscanf(f, "PROFONDEUR %d GROTTE %d\n", &prof, &grot) != 2) {
        fprintf(stderr, "Sauvegarde corrompue (PROFONDEUR/GROTTE).\n");
        fclose(f); return 0;
    }

    /* JOUEUR */
    int pv, pvmax, o2, o2max, fatigue, perles, bam, bax, bdef;
    if (fscanf(f, "JOUEUR %d %d %d %d %d %d %d %d %d\n",
               &pv, &pvmax, &o2, &o2max, &fatigue, &perles, &bam, &bax, &bdef) != 9) {
        fprintf(stderr, "Sauvegarde corrompue (JOUEUR).\n");
        fclose(f); return 0;
    }

    /* EQUIP */
    int eh, ec;
    if (fscanf(f, "EQUIP %d %d\n", &eh, &ec) != 2) {
        fprintf(stderr, "Sauvegarde corrompue (EQUIP).\n");
        fclose(f); return 0;
    }

    /* INV */
    int inv_count=0;
    if (fscanf(f, "INV %d\n", &inv_count) != 1 || inv_count < 0 || inv_count > INV_MAX) {
        fprintf(stderr, "Sauvegarde corrompue (INV).\n");
        fclose(f); return 0;
    }

    /* Réinitialiser l’inventaire avant de remplir */
    for (int i=0;i<INV_MAX;i++){ j->inv.slots[i].kind=0; j->inv.slots[i].subtype=0; j->inv.slots[i].qty=0; }

    for (int k=0;k<inv_count;k++){
        int kind, sub, qty;
        if (fscanf(f, "ITEM %d %d %d\n", &kind, &sub, &qty) != 3) {
            fprintf(stderr, "Sauvegarde corrompue (ITEM #%d).\n", k+1);
            fclose(f); return 0;
        }
        /* Try to place (pile consommables sinon 1 slot) */
        if (kind == ITEM_CONS) {
            /* même modèle que inv_add_consumable */
            int placed = 0;
            for (int i=0;i<INV_MAX;i++){
                if (j->inv.slots[i].kind==ITEM_CONS && j->inv.slots[i].subtype==sub){
                    j->inv.slots[i].qty += qty; placed=1; break;
                }
            }
            if (!placed){
                for (int i=0;i<INV_MAX;i++) if (!j->inv.slots[i].kind){
                        j->inv.slots[i].kind=ITEM_CONS; j->inv.slots[i].subtype=sub; j->inv.slots[i].qty=qty; placed=1; break;
                    }
            }
        } else {
            /* HARPON / COMBI → 1 exemplaire par slot */
            for (int i=0;i<INV_MAX;i++) if (!j->inv.slots[i].kind){
                    j->inv.slots[i].kind=kind; j->inv.slots[i].subtype=sub; j->inv.slots[i].qty=1; break;
                }
        }
    }

    /* Affecter les champs joueur */
    j->pv = clampi(pv, 0, pvmax);
    j->pv_max = pvmax;
    j->o2 = clampi(o2, 0, o2max);
    j->o2_max = o2max;
    j->fatigue = clampi(fatigue, 0, 5);
    j->perles = perles;
    j->base_atk_min = bam;
    j->base_atk_max = bax;
    j->base_defense = bdef;

    /* Équipements (bornage simple) */
    j->inv.eq_harpon = (eh >= HARPON_NONE && eh <= HARPON_LASER) ? (HarponType)eh : HARPON_ROUILLE;
    j->inv.eq_combi  = (ec >= COMBI_NONE  && ec <= COMBI_TITANIUM) ? (CombiType)ec : COMBI_NEOPRENE;

    *profondeur = prof;
    *en_grotte  = grot ? 1 : 0;

    fclose(f);
    printf("Sauvegarde chargée depuis '%s'.\n", path);
    return 1;
}

