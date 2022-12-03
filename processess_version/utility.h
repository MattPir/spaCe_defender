#ifndef UTILITY
#define UTILITY

/*this library provides strctures, inclusions and general functions*/

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SPRITE_LENGTH 5

//colors
#define BACKGROUND 0
#define PROJECTILE_COLOR 1
#define ENEMY_LV1_MAIN_COLOR 2
#define ENEMY_LV1_ACCENT_COLOR 3
#define ENEMY_LV2_MAIN_COLOR 4
#define ENEMY_LV2_ACCENT_COLOR 5
#define SPACESHIP_MAIN_COLOR 6
#define SPACESHIP_ACCENT_COLOR 7
#define LIFE ENEMY_LV2_MAIN_COLOR

//sprite's lengths
#define PLAYER_SPRITE_WITDH 5
#define PLAYER_SPRITE_HEIGHT 6
#define ENEMY_LV1_SPRITE_LENGTH 3
#define ENEMY_LV2_SPRITE_WIDTH 4
#define ENEMY_LV2_SPRITE_HEIGHT 3

//number of enemies
#define ENEMY_COLS 3
#define ENEMY_NUMBER ENEMY_COLS * enemyRows

//space between enemies at creation time
#define ENEMY_BLOCK_Y 10

//minimum screen lengths
#define MIN_WIDTH 140
#define MIN_HEIGHT ENEMY_LV2_SPRITE_HEIGHT + 1 + 3 * ENEMY_BLOCK_Y

typedef struct
{
    int xLeft, xRight, yUp, yDown;
} Position;

typedef struct
{
    Position position, oldpos;
    int id, processid;
    char sprite[SPRITE_LENGTH];
    bool shoot;
} Player;

typedef struct enemy
{
    Player enemyShip;
    struct enemy *next, *prev;
    int lifes;
} Enemy;

typedef struct
{
    int x, y;
} ScreenSize;

ScreenSize screen;
ScreenSize hudScreen;
int enemyRows;

int randomNumber(int min, int max);

int randomMovement(int maxUp, int maxDown);

void criticalError(char *errorDefinition);

void writeHorizontal(int n, int x, int y);

bool areColliding(Position element1, Position element2);

int getBlockNumber(int yPosition);

int outOfScreenProjectile(Position pos, int pid, char sprite[SPRITE_LENGTH]);

void deleteSprite(Position pos);

Enemy *find(Enemy *enemy, int pid);

int killpid(int pid);

Enemy *newEnemy(Enemy *);

Enemy *deleteEnemy(Enemy *, Enemy **list, const int index);

Position toComplete(int centralX, int centralY, int xLength, int yLength);

#endif