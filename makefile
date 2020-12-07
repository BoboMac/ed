all:
	clear && g++ -o ed.out main.cpp -lncurses -lX11 && ./ed.out main.cpp TODO.txt backend.h
