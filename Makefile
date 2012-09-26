OFILES         := socket.o
OFILES_CLIENT  := $(OFILES) main_client.o
OFILES_SERVER  := $(OFILES) main_server.o  server.o
OFILES_DISPLAY := $(OFILES) main_display.o display.o

CFLAGS  := -Wall -Wextra -Werror -pedantic -ansi -std=c99 -O3
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
	@echo " [RM] $(OFILES_SERVER) $(OFILES_CLIENT) $(OFILES_DISPLAY)"
	-@rm -f $(OFILES_SERVER) $(OFILES_CLIENT) $(OFILES_DISPLAY)

destroy: clean
	@echo " [RM] client server display"
	-@rm -f client server display

rebuild: destroy all
