GAME : 
	gcc 00GAME.c -lncurses -o GAME
run : savefile.txt
	./GAME
clean:
	rm GAME 