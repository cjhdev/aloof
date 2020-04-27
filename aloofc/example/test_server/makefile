DIR_ROOT := ../..
DIR_BUILD := build
DIR_BIN := bin

CC := gcc

VPATH += $(DIR_ROOT)/src

INCLUDES += -I$(DIR_ROOT)/include
INCLUDES += -I.

CFLAGS := -O0 -Wall -ggdb $(INCLUDES)

SRC := $(notdir $(wildcard $(DIR_ROOT)/src/*.c)) main.c

OBJ := $(SRC:.c=.o)

$(DIR_BIN)/test_server: $(addprefix $(DIR_BUILD)/, $(OBJ))
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(DIR_BUILD)/*

sqeaky_clean: clean
	rm -f $(DIR_BIN)/*

$(DIR_BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
