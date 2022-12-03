#include "utility.h"

/**
 * @brief generates a number in the interval [min, max]
 *
 */
int randomNumber(int min, int max)
{
    return min + rand() % (max - min + 1);
}

/**
 * @brief generates unitary movements in the y axis
 *
 * @param maxUp how far can you go Up
 * @param maxDown how far can you go Down
 * @return int -1 for going down, 1 for going up, 0 for staying still
 */
int randomMovement(int maxUp, int maxDown)
{
    int movement;

    movement = randomNumber(maxDown, maxUp);

    if (movement > 0)
        return 1;
    else if (movement < 0)
        return -1;
    else
        return 0;
}

/**
 * @brief closes the window and stops the execution because of a critical error
 *
 * @param errorDefinition message that defines the tipe of error
 */
void criticalError(char *errorDefinition)
{
    endwin();
    printf("\n%s\n", errorDefinition);
    kill(0, SIGKILL);
    exit(1);
}

/**
 * @brief writes n horizontal spaces in coordinates (y,x)
 * 
 * @param n how many spaces to write
 * @param x starting column 
 * @param y fixed row
 */
void writeHorizontal(int n, int x, int y)
{
    while (n > 0)
    {
        mvprintw(y, x, " ");
        x++;
        n--;
    }
}

/**
 * @brief checks if element1 and element2 are colliding each other
 *
 * @param element1 element prone to collide
 * @param element2 element that can collide
 * @return true if element1 and element2 are colliding
 * @return false if element1 and element2 aren't colliding
 */
bool areColliding(Position element1, Position element2)
{
    bool x, y;
    x = (element1.xRight >= element2.xLeft && element1.xRight <= element2.xRight) || (element2.xRight >= element1.xLeft && element2.xRight <= element1.xRight);
    y = (element1.yDown >= element2.yUp && element1.yDown <= element2.yDown) || (element2.yDown >= element1.yUp && element2.yDown <= element1.yDown);
    return x && y;
}

/**
 * @brief Get the Block Number object given his vertical position
 *
 * @param y position of the object in the y-axis
 * @return int block of the object
 */
int getBlockNumber(int y)
{
    int block = ((y - hudScreen.y) / ENEMY_BLOCK_Y) % enemyRows;
    return block;
}

/**
 * @brief deletes a sprite in the specified position
 *
 * @param pos the last wrote position in the screen
 */
void deleteSprite(Position pos)
{
    int x = pos.xRight - pos.xLeft + 1;

    attron(COLOR_PAIR(BACKGROUND));
    for (; pos.yDown - pos.yUp >= 0; pos.yDown--)
        writeHorizontal(x, pos.xLeft, pos.yDown);
    attroff(COLOR_PAIR(BACKGROUND));
}

/**
 * @brief kills projectile's process if it's out of screen
 *
 * @param pos current position of the projectile
 * @param pid projectile's process id
 * @param sprite projectile's sprite
 * @return int 1 if the projectile is killed, 0 if not
 */
int outOfScreenProjectile(Position pos, int pid, char sprite[SPRITE_LENGTH])
{
    if (pos.xLeft <= 0 || pos.xRight >= screen.x || pos.yUp <= hudScreen.y || pos.yDown >= screen.y) // out of screen
    {
        killpid(pid);
        return 1;
    }
    else
    {
        attron(COLOR_PAIR(PROJECTILE_COLOR));
        mvprintw(pos.yUp, pos.xLeft, "%s",sprite);
        attroff(COLOR_PAIR(PROJECTILE_COLOR));
        return 0;
    }
}

/**
 * @brief finds the corrispondent enemy in a row by pid
 *
 * @param enemy row of enemy to scan
 * @param pid identifier of the enemy
 * @return Enemy* with corrispondent pid || NULL if there is no enemy with that pid
 */
Enemy *find(Enemy *enemy, int pid)
{
    Enemy *tmp = enemy;
    while (tmp != NULL)
    {
        if (tmp->enemyShip.processid == pid)
            return tmp;
        tmp = tmp->next;
    }

    return NULL;
}

/**
 * @brief kills a process
 *
 * @param pid process id of the process to kill
 * @return int < 0 if kill errors
 */
int killpid(int pid)
{
    int error;
    error = kill(pid, SIGHUP);
    waitpid(pid, NULL, 0);
    return error;
}

/**
 * @brief creates a new enemy after element
 *
 * @param element element of the list
 * @return Enemy* new allocated enemy
 */
Enemy *newEnemy(Enemy *element)
{
    Enemy *tmp, *next;

    if (element->next == NULL)
    {
        tmp = (Enemy *)malloc(sizeof(Enemy));
        if (tmp == NULL)
            criticalError("Enemy not allocated in the list");
        tmp->prev = element;
        element->next = tmp;
        tmp->next = NULL;
        return tmp;
    }
    next = element->next;

    tmp = (Enemy *)malloc(sizeof(Enemy));
    if (tmp == NULL)
        criticalError("Enemy not allocated in the list");

    tmp->prev = element;
    tmp->next = next;
    next->prev = tmp;
    element->next = tmp;

    return tmp;
}

/**
 * @brief Deallocs an enemy from his list
 * 
 * @param tmp enemy to delete
 * @param list pointer array that contains all the enemies present on screen
 * @param x index of the array list
 * @return Enemy* new list without the deleted enemy
 */
Enemy *deleteEnemy(Enemy *tmp, Enemy **list, const int x)
{
    Enemy *next, *prev;

    if (tmp == NULL)
        criticalError("Enemy not in list has collided");

    prev = tmp->prev;
    next = tmp->next;

    refresh();

    if (next == NULL)
    {
        if (prev != NULL)
            prev->next = NULL;
        else
            list[x] = NULL;
    }
    else if (prev == NULL)
    {
        tmp = list[x];
        list[x] = list[x]->next;
        list[x]->prev = NULL;
    }
    else
    {
        prev->next = next;
        next->prev = prev;
    }
    free(tmp);

    return list[x];
}

/**
 * @brief converts a couple (x,y) to a Position structure
 * 
 * @param centralX x in the couple (x,y)
 * @param centralY y in the couple (x,y)
 * @param xLength with
 * @param yLength heigth
 * @return Position converted structure
 */
Position toComplete(int centralX, int centralY, int xLength, int yLength)
{
    Position pos;
    pos.yUp = centralY - yLength / 2;
    pos.xLeft = centralX - xLength / 2;
    pos.yDown = pos.yUp + yLength - 1;
    pos.xRight = pos.xLeft + xLength - 1;
    return pos;
}