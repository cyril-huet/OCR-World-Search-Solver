# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
EXEC_GRID = solver_grid
EXEC_LIST = solver_list
EXEC_CONTRACT = contract

# Cibles principales
.PHONY: all clean

all: $(EXEC_GRID) $(EXEC_LIST) $(EXEC_CONTRACT)

# Compilation des solvers
$(EXEC_GRID): solver_grid.c
	$(CC) $(CFLAGS) -o $(EXEC_GRID) solver_grid.c -lSDL2 -lSDL2_image -lm
	./$(EXEC_GRID)

$(EXEC_LIST): solver_list.c
	$(CC) $(CFLAGS) -o $(EXEC_LIST) solver_list.c -lSDL2 -lSDL2_image -lm
	./$(EXEC_LIST)

$(EXEC_CONTRACT): contract.c
	$(CC) $(CFLAGS) -o $(EXEC_CONTRACT) contract.c -lSDL2 -lSDL2_image -lm
	./$(EXEC_CONTRACT)
# Nettoyage
clean:
	rm -f $(EXEC_GRID) $(EXEC_LIST) $(EXEC_CONTRACT)

