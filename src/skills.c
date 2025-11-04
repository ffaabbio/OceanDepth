//
// Created by priss on 04/11/2025.
//

#include <skills.h>

const char* skill_name(SkillId id) {
    switch (id) {
        case SK_APNEE:         return "Apnee Prolongee";
        case SK_DECHARGE:      return "Decharge Electrique";
        case SK_COMMUNICATION: return "Communication Marine";
        case SK_TOURBILLON:    return "Tourbillon";
        default:               return "Inconnu";
    }
}
