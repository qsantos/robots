OFILES         := socket.o comm.o
OFILES_CLIENT  := $(OFILES) client.o
OFILES_SERVER  := $(OFILES) server.o
OFILES_DISPLAY := $(OFILES) display.o

CFLAGS  := -Wall -Wextra -pedantic -ansi -std=c99 -O3
LDFLAGS := -lm -lGL -lglfw -lSOIL

.PHONY: all clean destroy rebuild

all: client server display

client: $(OFILES_CLIENT)
	@echo " [LD] client"
	@gcc $^ -o $@ $(LDFLAGS)

server: $(OFILES_SERVER)
	@echo " [LD] server"
	@gcc $^ -o $@ $(LDFLAGS)

display: $(OFILES_DISPLAY)
	@echo " [LD] display"
	@gcc $^ -o $@ $(LDFLAGS)

%.o: %.c
	@echo " [CC] $^"
	@gcc $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(OFILES_SERVER) $(OFILES_CLIENT) $(OFILES_DISPLAY)

destroy: clean
	-rm -f client server display

rebuild: destroy all
