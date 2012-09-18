OFILES        := socket.o comm.o
OFILES_CLIENT := $(OFILES) client.o
OFILES_SERVER := $(OFILES) server.o

CFLAGS  := -Wall -O3 -Wextra -pedantic -ansi -std=c99
LDFLAGS := -lm

.PHONY: all clean destroy rebuild

all: client server

client: $(OFILES_CLIENT)
	@echo " [LD] client"
	@gcc $^ -o $@ $(LDFLAGS)

server: $(OFILES_SERVER)
	@echo " [LD] server"
	@gcc $^ -o $@ $(LDFLAGS)

%.o: %.c
	@echo " [CC] $^"
	@gcc $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(OFILES_SERVER) $(OFILES_CLIENT)

destroy:
	-rm -f $(OFILES_SERVER) $(OFILES_CLIENT) client server

rebuild: clean all
