all: chat clean

chat: main.o get_my_ip.o
	gcc main.o get_my_ip.o -o chat

main: main.c
	gcc -c main.c

get_my_ip: get_my_ip.c
	gcc -c get_my_ip.c

clean:
	rm -rf *.o
