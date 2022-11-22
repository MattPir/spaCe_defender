#ifndef GAME
#define GAME

/* Sets and manages the game execution */
#include "utility.h"
#include "player.h"


void startGame();

void gameFieldManager(Player *player, Enemy *enemies[]);

int hud(int lifes, int lv1Enemies, int lv2Enemies);

Enemy *createEnemies(Position, Enemy*);

#endif