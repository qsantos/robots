OFILES         := socket.o comm.o
OFILES_CLIENT  := $(OFILES) client.o
OFILES_SERVER  := $(OFILES) server.o
OFILES_DISPLAY := $(OFILES) display.o

CFLAGS  := -Wall -O3 -Wextra -pedantic -ansi -std=c99 -g
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
	-rm -f $(OFILES_SERVER) $(OFILES_CLIENT)

destroy:
	-rm -f $(OFILES_SERVER) $(OFILES_CLIENT) client server display

rebuild: clean all
