#include "game.h"

/**
 * @brief initialize the game
 * 
 */
void startGame()
{
    clear();
    enemyRows = (screen.y - ENEMY_LV2_SPRITE_HEIGHT + 1) / ENEMY_BLOCK_Y;
    hudScreen.y = hud(PLAYER_LIFES, enemyRows * ENEMY_COLS, 0);
    hudScreen.x = screen.x;

    if (hudScreen.y + enemyRows * ENEMY_BLOCK_Y + 1 < screen.y)
        screen.y = hudScreen.y + enemyRows * ENEMY_BLOCK_Y + 1;

    __pid_t playerPid, enemyPid;
    int p[2], n = 0, col = 0, row = -1;
    Position spawnPosition;
    Player player;
    Enemy *enemies[enemyRows], *tmp;

    if (pipe(p) <= -1)
        criticalError("Pipe not created");

    player.position.xLeft = 4;
    player.position.xRight = player.position.xLeft + PLAYER_SPRITE_WITDH - 1;
    player.position.yUp = screen.y / 2 - PLAYER_SPRITE_HEIGHT / 2;
    player.position.yDown = player.position.yUp + PLAYER_SPRITE_HEIGHT - 1;

    drawPlayer(player.position);

    playerPid = fork();
    if (playerPid < 0)
    {
        close(p[0]);
        close(p[1]);
        criticalError("Error: fork failed");
    }
    else if (playerPid == 0) // player created
    {
        close(p[0]);
        playerInput(p[1], player.position);
    }
    else
    {
        spawnPosition.yUp = hudScreen.y - (ENEMY_BLOCK_Y / 2) - 1;
        spawnPosition.yDown = spawnPosition.yUp + ENEMY_LV1_SPRITE_LENGTH - 1;

        while (n < ENEMY_NUMBER)
        {
            if (col == 0)
            {
                row++;
                enemies[row] = malloc(sizeof(Enemy));
                if (enemies[row] == NULL)
                    criticalError("Allocation failed");
                enemies[row]->prev = NULL;
                enemies[row]->next = NULL;
                tmp = enemies[row];
                spawnPosition.yUp += ENEMY_BLOCK_Y;
                spawnPosition.yDown = spawnPosition.yUp + ENEMY_LV1_SPRITE_LENGTH - 1;
            }
            else
            {
                tmp->next = newEnemy(tmp);
                tmp = tmp->next;
                tmp->next = NULL;
            }

            spawnPosition.xRight = screen.x - (30 * col + 1);
            spawnPosition.xLeft = spawnPosition.xRight - ENEMY_LV1_SPRITE_LENGTH + 1;
            drawLV1Enemy(spawnPosition);

            enemyPid = fork();
            if (enemyPid == 0)
            {
                close(p[0]);
                enemyAI(p[1], spawnPosition, 1, time(NULL) * getpid() + getpid());
            }
            else if (enemyPid < 0)
                criticalError("Fork() not executed for an enemy");

            tmp->enemyShip.processid = enemyPid;
            tmp->enemyShip.position = tmp->enemyShip.oldpos = spawnPosition;
            tmp->enemyShip.id = ENEMY_LV1_ID;
            tmp->lifes = 1;

            col = (col + 1) % ENEMY_COLS;
            n++;
        }

        refresh();
        gameFieldManager(p[0], p[1], &player, enemies);
        attroff(COLOR_PAIR(BACKGROUND));
        attron(COLOR_PAIR(SPACESHIP_MAIN_COLOR));
        mvprintw(screen.y - 1, 0, "Press any key to continue");
        attroff(COLOR_PAIR(SPACESHIP_MAIN_COLOR));
        attron(COLOR_PAIR(BACKGROUND));
        refresh();
        napms(100);
        getch();
        close(p[0]);
        close(p[1]);
    }
    clear();
    refresh();
}

/**
 * @brief Manages all the processes in game field
 * 
 * @param readPipe pipe used to read processes's writes
 * @param writePipe used to pass it to new processes
 * @param player structure that contains information about the player
 * @param enemies structure with information about all the enemies
 */
void gameFieldManager(int readPipe, int writePipe, Player *player, Enemy *enemies[])
{
    Player gpPlayer;
    Enemy *tmp;
    int x, playerLifes = PLAYER_LIFES, numberOfProjectiles = 0, numberOfLv1Enemies = ENEMY_NUMBER, numberOfLv2Enemies = 0, pid;
    bool playing = true;

    while (playing || numberOfProjectiles > 0 || numberOfLv1Enemies > 0 || numberOfLv2Enemies > 0)
    {
        read(readPipe, &gpPlayer, sizeof(Player));

        switch (gpPlayer.id)
        {
        case PLAYER_ID:
            if (!playing)
            {
                killpid(player->processid);
                break;
            }
            player->position = gpPlayer.position;
            player->oldpos = gpPlayer.oldpos;
            player->processid = gpPlayer.processid;
            deleteSprite(player->oldpos);
            drawPlayer(player->position);
            if (gpPlayer.shoot)
            {
                pid = fork();
                if (pid == 0)
                {
                    player->position = toComplete(player->position.xRight + 1, (player->position.yUp + player->position.yDown)/ 2, 1, 1);
                    shot(player->position, RIGHT, SHOOT_UP, PLAYER_PROJECTILE_ID, "o\0", writePipe);
                }
                else if (pid < 0)
                    criticalError("Fork failed in player up shot");

                numberOfProjectiles++;

                pid = fork();
                if (pid == 0)
                {
                    player->position = toComplete(player->position.xRight + 1, (player->position.yUp + player->position.yDown)/ 2, 1, 1);
                    shot(player->position, RIGHT, SHOOT_DOWN, PLAYER_PROJECTILE_ID, "o\0", writePipe);
                }
                else if (pid < 0)
                    criticalError("Fork failed in player down shot");

                numberOfProjectiles++;
            }
            break;

        case ENEMY_LV1_ID:
        case ENEMY_LV2_ID:
            x = getBlockNumber(gpPlayer.position.yDown);
            tmp = find(enemies[x], gpPlayer.processid);
            if (tmp == NULL)
                break;
            tmp->enemyShip.position = gpPlayer.position;
            tmp->enemyShip.oldpos = gpPlayer.oldpos;

            if (!playing)
            {
                killpid(tmp->enemyShip.processid);
                deleteEnemy(tmp, enemies, x);

                if (tmp->enemyShip.id == ENEMY_LV1_ID)
                    numberOfLv1Enemies--;
                else if (tmp->enemyShip.id == ENEMY_LV2_ID)
                    numberOfLv2Enemies--;
                else
                    criticalError("Enemy with not existent id found");
                break;
            }

            if (tmp->enemyShip.position.xLeft <= 0)
            {
                playing = false;
                killpid(tmp->enemyShip.processid);
                deleteSprite(tmp->enemyShip.oldpos);
                deleteEnemy(tmp, enemies, x);
                break;
            }
            else if (gpPlayer.shoot)
            {
                pid = fork();
                if (pid == 0)
                {
                    tmp->enemyShip.position = toComplete(tmp->enemyShip.position.xRight - 1, (tmp->enemyShip.position.yUp + tmp->enemyShip.position.yDown)/ 2, 1, 1);
                    shot(tmp->enemyShip.position, LEFT, SHOOT_HORIZONTAL, ENEMY_PROJECTILE_ID, "-\0", writePipe);
                }
                else if (pid < 0)
                    criticalError("Fork failed in enemy shot");

                numberOfProjectiles++;
            }

            //break if it tries to write too up or too far from his previous position
            if(gpPlayer.position.xLeft - gpPlayer.oldpos.xLeft > LEFT_SHIFT || gpPlayer.position.yUp < hudScreen.y)
                break;

            deleteSprite(tmp->enemyShip.oldpos);
            if (gpPlayer.id == ENEMY_LV1_ID)
                drawLV1Enemy(tmp->enemyShip.position);
            else if (gpPlayer.id == ENEMY_LV2_ID)
                drawLV2Enemy(tmp->enemyShip.position);

            break;

        case PLAYER_PROJECTILE_ID:
            if (!playing)
            {
                killpid(gpPlayer.processid);
                numberOfProjectiles--;
                mvprintw(gpPlayer.oldpos.yUp, gpPlayer.oldpos.xLeft, " ");
                break;
            }

            x = getBlockNumber(gpPlayer.position.yUp);
            tmp = enemies[x];

            if (tmp == NULL)
                numberOfProjectiles -= outOfScreenProjectile(gpPlayer.position, gpPlayer.processid, gpPlayer.sprite);
            else
            {
                while (tmp != NULL)
                {
                    if (areColliding(gpPlayer.position, tmp->enemyShip.position)) // collision
                    {
                        tmp->lifes--;
                        killpid(gpPlayer.processid);
                        numberOfProjectiles--;

                        if (tmp->lifes <= 0)
                        {
                            killpid(tmp->enemyShip.processid);
                            deleteSprite(tmp->enemyShip.position);
                            if (tmp->enemyShip.id == ENEMY_LV1_ID)
                            {
                                numberOfLv1Enemies--;
                                tmp->next = createEnemies(tmp->enemyShip.position, tmp, writePipe);
                                numberOfLv2Enemies += 4;
                            }
                            else if (tmp->enemyShip.id == ENEMY_LV2_ID)
                                numberOfLv2Enemies--;
                            else
                                criticalError("AN UNKNOW ENEMY HAS BEEN KILLED");
                            enemies[x] = deleteEnemy(tmp, enemies, x);
                        }
                        else
                        {
                            mvprintw(gpPlayer.oldpos.yUp, gpPlayer.oldpos.xLeft, " ");
                            refresh();
                        }

                        if (numberOfLv1Enemies == 0 && numberOfLv2Enemies == 0)
                            playing = false;

                        gpPlayer.position = gpPlayer.oldpos;
                        hud(playerLifes, numberOfLv1Enemies, numberOfLv2Enemies);
                        break;
                    }
                    else
                    {
                        if (outOfScreenProjectile(gpPlayer.position, gpPlayer.processid, gpPlayer.sprite) == 1)
                        {
                            numberOfProjectiles--;
                            tmp = NULL;
                        }
                    }

                    if (tmp != NULL)
                        tmp = tmp->next;
                }
            }
            mvprintw(gpPlayer.oldpos.yUp, gpPlayer.oldpos.xLeft, " ");
            break;

        case ENEMY_PROJECTILE_ID:
            if (!playing)
            {
                killpid(gpPlayer.processid);
                numberOfProjectiles--;
                mvprintw(gpPlayer.oldpos.yUp, gpPlayer.oldpos.xLeft, " ");
                break;
            }

            if (areColliding(player->position, gpPlayer.position) && playerLifes > 0)
            {
                killpid(gpPlayer.processid);
                numberOfProjectiles--;
                playerLifes--;
                hud(playerLifes, numberOfLv1Enemies, numberOfLv2Enemies);

                if (playerLifes == 0)
                {
                    playing = false;
                    killpid(player->processid);
                    deleteSprite(player->position);
                }
                else
                    drawPlayer(player->position);
            }
            else
                numberOfProjectiles -= outOfScreenProjectile(gpPlayer.position, gpPlayer.processid, gpPlayer.sprite);
            mvprintw(gpPlayer.oldpos.yUp, gpPlayer.oldpos.xLeft, " ");

            // rewrites enemies of the same line to prevent sprite to be only partiallly deleted
            x = getBlockNumber(gpPlayer.position.yDown);
            tmp = enemies[x];

            while (tmp != NULL)
            {
                //prevents unwanted writes to the window
                if (tmp->enemyShip.position.xLeft != 0 && tmp->enemyShip.position.yUp >= hudScreen.y)
                {
                    if (tmp->enemyShip.id == ENEMY_LV1_ID)
                        drawLV1Enemy(tmp->enemyShip.position);
                    else if (tmp->enemyShip.id == ENEMY_LV2_ID)
                        drawLV2Enemy(tmp->enemyShip.position);
                    else
                        criticalError("Enemy with wrong id in enemies");
                }
                tmp = tmp->next;
            }
            break;
        }

        refresh();
    }
    if (playerLifes == 0)
    {
        /*Game over*/
        mvprintw(screen.y / 4 + 0, screen.x / 2 - (61 / 2) + 2, "________");
        mvprintw(screen.y / 4 + 1, screen.x / 2 - (61 / 2) + 1, "/  _____/_____    _____   ____     _______  __ ___________");
        mvprintw(screen.y / 4 + 2, screen.x / 2 - (61 / 2), "/   \\  ___\\__  \\  /     \\_/ __ \\   /  _ \\  \\/ // __ \\_  __ \\");
        mvprintw(screen.y / 4 + 3, screen.x / 2 - (61 / 2), "\\    \\_\\  \\/ __ \\|  Y Y  \\  ___/  (  <_> )   /\\  ___/|  | \\/");
        mvprintw(screen.y / 4 + 4, screen.x / 2 - (61 / 2) + 1, "\\______  (____  /__|_|  /\\___  >  \\____/ \\_/  \\___  >__|");
        mvprintw(screen.y / 4 + 5, screen.x / 2 - (61 / 2) + 8, "\\/     \\/      \\/     \\/                   \\/");
    }
    else if (numberOfLv1Enemies == 0 && numberOfLv2Enemies == 0)
    {
        /*!! VICTORY !!*/
        mvprintw(screen.y / 4 + 0, screen.x / 2 - (54 / 2), " _  _    _  _  __  ___  ____  __  ____  _  _    _  _         ");
        mvprintw(screen.y / 4 + 1, screen.x / 2 - (54 / 2), "/ \\/ \\  / )( \\(  )/ __)(_  _)/  \\(  _ \\( \\/ )  / \\/ \\");
        mvprintw(screen.y / 4 + 2, screen.x / 2 - (54 / 2), "\\_/\\_/  \\ \\/ / )(( (__   )( (  O ))   / )  /   \\_/\\_/  ");
        mvprintw(screen.y / 4 + 3, screen.x / 2 - (54 / 2), "(_)(_)   \\__/ (__)\\___) (__) \\__/(__\\_)(__/    (_)(_)    ");
    }
    else
        criticalError("Game ended without a reason");
}

/**
 * @brief writes an HUD on top of the screen
 * 
 * @param lifes number of lifes of the player
 * @param lv1Enemies number of lv1 enemy alive
 * @param lv2Enemies number of lv2 enemy alive
 * @return int number of line used for the HUD
 */
int hud(int lifes, int lv1Enemies, int lv2Enemies)
{
    Position pos;
    int x = 1, y = 1, i = 0;
    char *life = "       ";

    // drawing lifes
    attroff(COLOR_PAIR(BACKGROUND));
    attron(COLOR_PAIR(LIFE));
    while (i < lifes)
    {
        mvprintw(y, x, "%s",life);
        x += strlen(life) + 1;
        i++;
    }
    attroff(COLOR_PAIR(LIFE));
    attron(COLOR_PAIR(BACKGROUND));
    while (i < PLAYER_LIFES)
    {
        mvprintw(y, x, "%s",life);
        x += strlen(life) + 1;
        i++;
    }

    // drawing number of lv1 enemies
    pos.yUp = 0;
    pos.yDown = pos.yUp + ENEMY_LV1_SPRITE_LENGTH - 1;
    pos.xLeft = 70 * screen.x / 100;
    pos.xRight = pos.xLeft + ENEMY_LV1_SPRITE_LENGTH - 1;
    drawLV1Enemy(pos);
    mvprintw(1, pos.xRight + 1, "x%d ", lv1Enemies);

    // drawing number of lv2 enemies
    pos.yUp = 0;
    pos.yDown = pos.yUp + ENEMY_LV2_SPRITE_HEIGHT - 1;
    pos.xLeft = 90 * screen.x / 100;
    pos.xRight = pos.xLeft + ENEMY_LV2_SPRITE_WIDTH - 1;
    drawLV2Enemy(pos);
    mvprintw(1, pos.xRight + 1, "x%d ", lv2Enemies);

    attron(COLOR_PAIR(BACKGROUND));
    for (i = 0; i < screen.x; i++)
        mvprintw(ENEMY_LV2_SPRITE_HEIGHT, i, "_");

    return ENEMY_LV2_SPRITE_HEIGHT + 1;
}

/**
 * @brief Create a Enemies object
 * 
 * @param pipe 
 * @return Enemy* 
 */
Enemy *createEnemies(Position pos, Enemy *element, int pipe)
{
    int n = 4, j = 2, row = -1, col = -1, pid;
    const int x = (pos.xLeft + pos.xRight) / 2, y = hudScreen.y + ENEMY_BLOCK_Y * getBlockNumber(pos.yUp) + ENEMY_BLOCK_Y / 2,
              z = ENEMY_LV2_SPRITE_WIDTH + 1, k = ENEMY_BLOCK_Y / 4;
    unsigned int seed = time(NULL) * time(NULL);
    Enemy *tmp = element;

    while (n > 0)
    {
        while (n > j)
        {
            pos = toComplete(x + (col * z), y + (row * k), ENEMY_LV2_SPRITE_WIDTH, ENEMY_LV2_SPRITE_HEIGHT);
            pid = fork();
            if (pid == 0)
            {
                enemyAI(pipe, pos, 2, seed);
            }
            else if (pid < 0)
                criticalError("EnemyLV2 not created");

            tmp->next = newEnemy(tmp);
            tmp = tmp->next;
            tmp->enemyShip.processid = pid;
            tmp->enemyShip.id = ENEMY_LV2_ID;
            tmp->lifes = 2;
            row = -row;
            n--;
        }
        j = 0;
        col = -col;
    }

    return element->next;
}