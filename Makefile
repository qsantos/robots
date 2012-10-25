OFILES         := socket.o
OFILES_CLIENT  := $(OFILES) main_client.o
OFILES_SERVER  := $(OFILES) main_server.o server.o game.o
OFILES_DISPLAY := $(OFILES) main_display.o

CFLAGS  := -Wall -Wextra -Werror -pedantic -ansi -std=c99 -O3 -D_XOPEN_SOURCE=500
LDFLAGS := -lm -lGL -lglut -lSOIL

.PHONY: all clean destroy rebuild

all: client server display

client: $(OFILES_CLIENT)
	gcc $^ -o $@ $(LDFLAGS)

server: $(OFILES_SERVER)
	gcc $^ -o $@ $(LDFLAGS)

display: $(OFILES_DISPLAY)
	gcc $^ -o $@ $(LDFLAGS)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OFILES_SERVER) $(OFILES_CLIENT) $(OFILES_DISPLAY)

destroy: clean
	rm -f client server display

rebuild: destroy all
