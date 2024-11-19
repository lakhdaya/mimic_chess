CC = gcc
CFLAGS = -W -Wall -ansi -std=c99 -lg
LDFLAGS = -lMLV -lpython3.10
EXEC = chess
SRC_DIR = chess/code
OBJ_DIR = obj
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)

chess : main clean

main : $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

main.o : pieces.h files.h display.h graphics.h engine.h games.h

%.o: %.c
		$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

clean: 
		@rm -rf $(SRC_DIR)/*.o

mrproper: clean
		@rm -rf $(EXEC)
