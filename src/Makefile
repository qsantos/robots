CFLAGS = -Wall -Wextra -Werror -pedantic -ansi -std=c99 -O3 -D_XOPEN_SOURCE=500
BINDIR = ../bin
OBJDIR = ../obj

SRC_SER = $(wildcard *.c server/*.c)
SRC_DIS = $(wildcard *.c display/*.c)
SRC_CLI = $(wildcard *.c client/*.c)

OBJ_SER = $(addprefix $(OBJDIR)/, $(SRC_SER:.c=.o))
OBJ_DIS = $(addprefix $(OBJDIR)/, $(SRC_DIS:.c=.o))
OBJ_CLI = $(addprefix $(OBJDIR)/, $(SRC_CLI:.c=.o))

all: $(addprefix $(BINDIR)/, client server display)

$(BINDIR)/server: $(OBJ_SER)
	@echo "Linking server..."
	@mkdir -p $(@D)
	@gcc $^ -lm -o $@
	@echo ""

$(BINDIR)/display: $(OBJ_DIS)
	@echo "Linking display..."
	@mkdir -p $(@D)
	@gcc $^ -lm -lpthread -lGL -lglut -lSOIL -lopenal -lalut -lvorbisfile -o $@
	@echo ""

$(BINDIR)/client: $(OBJ_CLI)
	@echo "Linking client..."
	@mkdir -p $(@D)
	@gcc $^ -lm -lpthread -o $@
	@echo ""

$(OBJDIR)/%.o: %.c
	@echo $<
	@mkdir -p $(@D)
	@gcc $(CFLAGS) -c $< -o $@

clean:
	rm -Rf $(OBJDIR)

destroy: clean
	rm -Rf $(BINDIR)

rebuild: destroy all
