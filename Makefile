OFILES_CLIENT  = socket.o main_client.o
OFILES_SERVER  = socket.o main_server.o server.o game.o
OFILES_DISPLAY = socket.o main_display.o game.o

CFLAGS  = -Wall -Wextra -Werror -pedantic -ansi -std=c99 -O3 -D_XOPEN_SOURCE=500 -g
LDFLAGS = -lm -lGL -lglut -lSOIL

all: client server display

client: $(OFILES_CLIENT)
	gcc $(LDFLAGS) $^ -o $@

server: $(OFILES_SERVER)
	gcc $(LDFLAGS) $^ -o $@

display: $(OFILES_DISPLAY)
	gcc $(LDFLAGS) $^ -o $@

%.o: %.c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OFILES_SERVER) $(OFILES_CLIENT) $(OFILES_DISPLAY)

destroy: clean
	rm -f client server display

rebuild: destroy all
