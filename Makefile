battleships :
	gcc battleships.c -lncurses -o battleships
run : 
	./battleships my_board.txt; rm battleships
