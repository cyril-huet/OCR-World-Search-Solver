# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Werror

# Target executable name
EXEC_SOLVER = solver

# Default rule (all)
all: $(EXEC_SOLVER)

# Compile solver
$(EXEC_SOLVER): solver.c
	$(CC) $(CFLAGS) -o $(EXEC_SOLVER) solver.c

# Clean up executable files
.PHONY: clean
clean:
	rm -rf $(EXEC_SOLVER) solver.dSYM

