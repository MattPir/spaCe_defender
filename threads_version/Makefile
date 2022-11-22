space: main.o mainMenu.o game.o player.o utility.o
	cc -o SpaceDefender main.o mainMenu.o game.o player.o utility.o -lncurses -lpthread

main.o: main.c
	cc -c main.c

mainMenu.o: mainMenu.c mainMenu.h
	cc -c mainMenu.c mainMenu.h

game.o: game.c game.h
	cc -c game.c game.h

player.o: player.c player.h
	cc -c player.c player.h

utility.o: utility.c utility.h
	cc -c utility.c utility.h

clean:
	rm -f *.o *.h.gch