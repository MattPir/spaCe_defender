#include "mainMenu.h"

/**
 * @brief Main menu of the game
 * 
 */
void menu()
{
    char choose = 'P';
    attron(COLOR_PAIR(BACKGROUND));

    do //forces the user to have a minimum size for the window
    {
        getmaxyx(stdscr, screen.y, screen.x);
        mvprintw(screen.y / 2, screen.x / 2, "The window is too small, minimum dimensions %d x %d", MIN_WIDTH, MIN_HEIGHT);

        if (screen.y >= MIN_HEIGHT)
        {
            if (screen.x < MIN_WIDTH)
                mvprintw(screen.y / 2 + 1, screen.x / 2, "Please increment the witdh of the window");
        }
        else if (screen.x >= MIN_WIDTH)
            mvprintw(screen.y / 2 + 1, screen.x / 2, "Please increment the height of the window");
        else
            mvprintw(screen.y / 2 + 1, screen.x / 2, "Please increment both the witdh and the height of the window");

        refresh();
        clear();
        napms(200);
    } while (screen.x < MIN_WIDTH || screen.y < MIN_HEIGHT);

    do
    {
        clear();
        
        /* Space Defender */
        mvprintw(screen.y / 4 + 0, screen.x / 2 - (83 / 2), " ___________  ___  _____  _____   ____  _____ _____ _____ _   _ _____ _____ _____              ");
        mvprintw(screen.y / 4 + 1, screen.x / 2 - (83 / 2), "/  ___| ___ \\/ _ \\/  __ \\|  ___| |  _ \\|  ___|  ___|  ___| \\ | |  _  \\  ___| ___ \\      ");
        mvprintw(screen.y / 4 + 2, screen.x / 2 - (83 / 2), "\\ `--.| |_/ / /_\\ \\ /  \\/| |__   | | | | |__ | |_  | |__ |  \\| | | | | |__ | |_/ /        ");
        mvprintw(screen.y / 4 + 3, screen.x / 2 - (83 / 2), " `--. \\  __/|  _  | |    |  __|  | | | |  __||  _| |  __|| . ` | | | |  __||    /             ");
        mvprintw(screen.y / 4 + 4, screen.x / 2 - (83 / 2), "/\\__/ / |   | | | | \\__/\\| |___  | |/ /| |___| |   | |___| |\\  | |/ /| |___| |\\ \\        ");
        mvprintw(screen.y / 4 + 5, screen.x / 2 - (83 / 2), "\\____/\\_|   \\_| |_/\\____/\\____/  |___/ \\____/\\_|   \\____/\\_| \\_/___/ \\____/\\_| \\_|");
        
        mvprintw(screen.y / 2, screen.x / 2 - (12 / 2), "1) NEW GAME");
        mvprintw(screen.y / 2 + 2, screen.x / 2 - (12 / 2), "2) EXIT");

        refresh();
        attroff(COLOR_PAIR(BACKGROUND));

        timeout(100);
        choose = getch();
        getmaxyx(stdscr, screen.y, screen.x);
        switch (choose)
        {
        case '1':
        case SPACE:
            timeout(-1);
            startGame();
            break;

        case '2':
        case 'q':
        case 'Q':
            break;

        default:
            break;
        }
    } while (choose != '2');
}