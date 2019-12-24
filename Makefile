build: GAME
	gcc 00GAME.c -lncurses -lpanel -o GAME
run:
	./GAME
clean:
	rm GAME