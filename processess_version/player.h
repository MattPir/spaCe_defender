#ifndef PLAYER
#define PLAYER

#include "utility.h"
/* Manages the player inputs*/

//keys
#define W 'w' : case 'W'
#define S 's' : case 'S'
#define E 'e' : case 'E'
#define SPACE ' '

//movements
#define RIGHT 1
#define LEFT -1

#define SHOOT_UP -0.1
#define SHOOT_DOWN 0.1
#define SHOOT_HORIZONTAL 0

#define MAX_MOVEMENT 2
#define LEFT_SHIFT 3

//probabilities
#define ENEMY_SHOT_PROBABILITY 5

//ids
#define PLAYER_ID 99
#define ENEMY_LV1_ID 33
#define ENEMY_LV2_ID ENEMY_LV1_ID * 2
#define PLAYER_PROJECTILE_ID 48
#define ENEMY_PROJECTILE_ID 52

//lifes
#define PLAYER_LIFES 9
#define ENEMY_LIFES 1

//delays
#define SHIFT_DELAY 5
#define PLAYER_SHOOT_DELAY 0.5
#define ENEMY_DELAY 800
#define PROJECTYLE_DELAY 20


void playerInput(int pipe, Position coordinates);

void enemyAI(int pipeWrite, Position coordinates, const int enemyLevel, const unsigned int seed);

void shot(Position, int direction, double diagonal, int id, char *sprite, int pipeWrite);

Player initPlayer(Position, int id, char *sprite);

void drawPlayer(Position currentPosition);

void drawLV1Enemy(Position currentPosition);

void drawLV2Enemy(Position currentPosition);


#endif