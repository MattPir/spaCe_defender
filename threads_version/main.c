#include "mainMenu.h"

int main()
{
    initscr();     // Inits the screen
    start_color(); // enables the colors
    noecho();      // sets the keyboard mode
    curs_set(false);   // hides the cursor
    keypad(stdscr, true);

    init_pair(PROJECTILE_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(ENEMY_LV1_MAIN_COLOR, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(ENEMY_LV1_ACCENT_COLOR, COLOR_BLACK, COLOR_GREEN);
    init_pair(ENEMY_LV2_MAIN_COLOR, COLOR_WHITE, COLOR_RED);
    init_pair(ENEMY_LV2_ACCENT_COLOR, COLOR_CYAN, COLOR_CYAN);
    init_pair(SPACESHIP_MAIN_COLOR, COLOR_BLACK, COLOR_WHITE);
    init_pair(SPACESHIP_ACCENT_COLOR, COLOR_WHITE, COLOR_CYAN);

    
    bkgd(COLOR_PAIR(BACKGROUND));

    
    menu();

    refresh();
    endwin(); /* deallocs the window */
    return 0;
}