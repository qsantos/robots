CFLAGS = -Wall -Wextra -Werror -pedantic -ansi -std=c99 -O3 -D_XOPEN_SOURCE=500
BINDIR = bin
OBJDIR = obj

SRC_SER = $(wildcard *.c server/*.c)
SRC_DIS = $(wildcard *.c display/*.c)
SRC_CLI = $(wildcard *.c client/*.c)

OBJ_SER = $(addprefix $(OBJDIR)/, $(SRC_SER:.c=.o))
OBJ_DIS = $(addprefix $(OBJDIR)/, $(SRC_DIS:.c=.o))
OBJ_CLI = $(addprefix $(OBJDIR)/, $(SRC_CLI:.c=.o))

all: $(BINDIR)/client $(BINDIR)/server $(BINDIR)/display

$(BINDIR)/server: $(OBJ_SER)
	@echo $@
	@mkdir -p $(@D)
	@gcc -lm $^ -o $@

$(BINDIR)/display: $(OBJ_DIS)
	@echo $@
	@mkdir -p $(@D)
	@gcc -lGL -lglut -lSOIL -lalut -lvorbisfile $^ -o $@

$(BINDIR)/client: $(OBJ_CLI)
	@echo $@
	@mkdir -p $(@D)
	@gcc -lm -lpthread $^ -o $@

$(OBJDIR)/%.o: %.c
	@echo $<
	@mkdir -p $(@D)
	@gcc $(CFLAGS) -c $< -o $@

clean:
	rm -Rf $(OBJDIR)

destroy: clean
	rm -Rf $(BINDIR)

rebuild: destroy all
