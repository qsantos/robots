all: client server display

client: socket.o main_client.o
	gcc $^ -o $@

server: socket.o main_server.o server.o game.o
	gcc -lm $^ -o $@

display: socket.o simpleOGG.o main_display.o
	gcc -lGL -lglut -lSOIL -lalut -lvorbisfile $^ -o $@

%.o: %.c
	gcc -Wall -Wextra -Werror -pedantic -ansi -std=c99 -O3 -D_XOPEN_SOURCE=700 -c $<

clean:
	rm -f *.o

destroy: clean
	rm -f client server display

rebuild: destroy all
