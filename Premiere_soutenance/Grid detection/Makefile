# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -g
EXEC_DETECTION = grid_detection
EXEC_SEPARATION = grid_and_list_separation

# Default rule (all)
all: $(EXEC_DETECTION) $(EXEC_SEPARATION)

# Compile grid_detection
$(EXEC_DETECTION): grid_detection.c
	$(CC) $(CFLAGS) -o $(EXEC_DETECTION) grid_detection.c -lSDL2 -lSDL2_image -lm

# Compile grid_and_list_separation
$(EXEC_SEPARATION): grid_and_list_separation.c
	$(CC) $(CFLAGS) -o $(EXEC_SEPARATION) grid_and_list_separation.c -lSDL2 -lSDL2_image -lm

# Clean up executable files
clean:
	rm -f $(EXEC_DETECTION) $(EXEC_SEPARATION)

