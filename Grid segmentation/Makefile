# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -g
EXEC_SEGMENTATION = grid_segmentation

# Default rule (all)
all: $(EXEC_SEGMENTATION)

# Compile grid_segmentation
$(EXEC_SEGMENTATION): grid_segmentation.c
	$(CC) $(CFLAGS) -o $(EXEC_SEGMENTATION) grid_segmentation.c

# Clean up executable and debug files
clean:
	rm -rf $(EXEC_SEGMENTATION) grid_segmentation.dSYM

