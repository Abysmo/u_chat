CC=gcc
CFLAGS=-Wall -c
LFLAGS=-lncurses

all: chat clean

chat: main.o get_my_ip.o terminal_ui.o
	$(CC) main.o get_my_ip.o terminal_ui.o $(LFLAGS) -o chat

main.o: main.c
	$(CC) $(CFLAGS) $(LFLAGS) main.c

get_my_ip.o: get_my_ip.c
	$(CC) $(CFLAGS) get_my_ip.c

terminal_ui.o: terminal_ui.c
	$(CC) $(CFLAGS) $(LFLAGS) terminal_ui.c

clean:
	rm -rf *.o
