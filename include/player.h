#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int healthPoints;
    int maxHealthPoints;
    int oxygenLevel;
    int maxOxygenLevel;
    int fatigueLevel; // 0 to 5
    int pearls;
} Player;

void displayPlayer(const Player *p);

#endif