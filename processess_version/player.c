#include "player.h"

/**
 * @brief records the input of the player and performs various actions
 *
 * @param pipeWrite is the pipe used for comunicating the player's input with the gameFieldManager
 * @param coordinates identify the player starting position on the screen
 */
void playerInput(int pipeWrite, Position coordinates)
{
    int input, shoot;
    time_t shotTime = 0, now;
    Player player = initPlayer(coordinates, PLAYER_ID, "P\0");

    write(pipeWrite, &player, sizeof(Player));

    while (true)
    {
        player.oldpos = player.position;
        input = getch();

        switch (input)
        {
        case KEY_UP:
        case W:
            if (player.position.yUp != hudScreen.y + 1)
            {
                player.position.yUp--;
                player.position.yDown--;
            }
            break;

        case KEY_DOWN:
        case S:
        
            if (player.position.yDown != screen.y - 1)
            {
                player.position.yUp++;
                player.position.yDown++;
            }
            break;
        case E:
        case SPACE:
            if (difftime(time(&now), shotTime) <= PLAYER_SHOOT_DELAY) // if the difference between the last shot and now is less than 2seconds don't shoot
                break;

            player.shoot = true;

            time(&shotTime);
            break;
        }

        write(pipeWrite, &player, sizeof(Player));
        player.shoot = false;
    }
}

/**
 * @brief defines npc's actions
 *
 * @param pipeWrite pipe used to communicate with gameFieldManager
 * @param coordinates starting point on the screen
 * @param enemyLevel enemy's level
 */
void enemyAI(int pipeWrite, Position coordinates, const int enemyLevel, const unsigned int seed)
{
    int relativeMovement = 0, movement = 0;
    time_t now, leftMovementDelay = 0;

    Player enemy = initPlayer(coordinates, ENEMY_LV1_ID * enemyLevel, "E\0");
    srand(seed);

    while (true)
    {
        enemy.oldpos = enemy.position;
        movement = randomMovement(MAX_MOVEMENT / enemyLevel - relativeMovement, -MAX_MOVEMENT / enemyLevel - relativeMovement);
        relativeMovement += movement;
        enemy.position.yUp += movement;
        enemy.position.yDown += movement;

        if (difftime(time(&now), leftMovementDelay) >= SHIFT_DELAY)
        {
            enemy.position.xLeft -= LEFT_SHIFT;
            enemy.position.xRight -= LEFT_SHIFT;
            time(&leftMovementDelay);
        }

        if (randomNumber(0, 100) < ENEMY_SHOT_PROBABILITY * enemyLevel)
            enemy.shoot = true;

        write(pipeWrite, &enemy, sizeof(Player));
        enemy.shoot = false;
        napms(ENEMY_DELAY);
    }
}

/**
 * @brief manages the projectile movements
 *
 * @param projectile is the object that rapresents projectiles in the game
 * @param direction where the projectile must move relatively to his current position
 * @param diagonal determines the movement in the Y-axis
 * @param pipeWrite is the pipe used for comunicating the player's input with the gameFieldManager
 */
void shot(Position pos, int direction, double diagonal, int id, char *sprite, int pipeWrite)
{
    Player projectile = initPlayer(pos, id, sprite);
    double yadder = 0;

    while (true)
    {
        projectile.oldpos = projectile.position;
        yadder += diagonal;
        projectile.position.xLeft += direction;
        projectile.position.xRight = projectile.position.xLeft;
        projectile.position.yUp += (int)yadder;
        projectile.position.yDown = projectile.position.yUp;
        write(pipeWrite, &projectile, sizeof(Player));
        if (yadder >= 1 || yadder <= -1)
            yadder = 0;
        napms(PROJECTYLE_DELAY);
    }
}

/**
 * @brief Initialize gamer, enemies and projectiles
 *
 * @param pos Player start position
 * @param lifes Player's number of lifes
 * @param id Player's type identificator
 * @param sprite Player's graphical rappresentation
 * @return Player
 */
Player initPlayer(Position pos, int id, char *sprite)
{
    Player player;
    player.position = player.oldpos = pos;
    player.id = id;
    player.processid = getpid();
    strcpy(player.sprite, sprite);
    player.shoot = false;

    return player;
}

/**
 * @brief writes the sprite of the player in the Position pos as shown in the file Sprites.png
 */
void drawPlayer(Position pos)
{
    attron(COLOR_PAIR(SPACESHIP_MAIN_COLOR));
    writeHorizontal(4, pos.xLeft, pos.yUp + 2);
    writeHorizontal(4, pos.xLeft, pos.yDown - 2);
    writeHorizontal(3, pos.xLeft + 1, pos.yUp);
    writeHorizontal(3, pos.xLeft + 1, pos.yDown);
    writeHorizontal(3, pos.xLeft + 2, pos.yUp + 1);
    writeHorizontal(3, pos.xLeft + 2, pos.yDown - 1);
    attroff(COLOR_PAIR(SPACESHIP_MAIN_COLOR));
    attron(COLOR_PAIR(SPACESHIP_ACCENT_COLOR));
    writeHorizontal(1, pos.xRight, pos.yUp + 2);
    writeHorizontal(1, pos.xRight, pos.yDown - 2);
    attroff(COLOR_PAIR(SPACESHIP_ACCENT_COLOR));
}

/**
 * @brief writes the sprite of a level 1 enemy in the Position pos as shown in the file Sprites.png
 */
void drawLV1Enemy(Position pos)
{
    attron(COLOR_PAIR(ENEMY_LV1_MAIN_COLOR));
    writeHorizontal(2, pos.xLeft, pos.yUp);
    writeHorizontal(2, pos.xLeft, pos.yDown);
    writeHorizontal(1, pos.xRight, pos.yUp + 1);
    attroff(COLOR_PAIR(ENEMY_LV1_MAIN_COLOR));
    attron(COLOR_PAIR(ENEMY_LV1_ACCENT_COLOR));
    writeHorizontal(1, pos.xLeft + 1, pos.yDown - 1);
    attroff(COLOR_PAIR(ENEMY_LV1_ACCENT_COLOR));
}

/**
 * @brief writes the sprite of a level 2 enemy in the Position pos as shown in the file Sprites.png
 */
void drawLV2Enemy(Position pos)
{
    attron(COLOR_PAIR(ENEMY_LV2_MAIN_COLOR));
    writeHorizontal(1, pos.xLeft, pos.yUp);
    writeHorizontal(1, pos.xLeft, pos.yDown);
    writeHorizontal(2, pos.xLeft + 2, pos.yUp);
    writeHorizontal(2, pos.xLeft + 2, pos.yDown);
    attroff(COLOR_PAIR(ENEMY_LV2_MAIN_COLOR));
    attron(COLOR_PAIR(ENEMY_LV2_ACCENT_COLOR));
    writeHorizontal(2, pos.xLeft + 1, pos.yUp + 1);
    attroff(COLOR_PAIR(ENEMY_LV2_ACCENT_COLOR));
}
