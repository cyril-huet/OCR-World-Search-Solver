# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -g
EXEC_NEURAL_NETWORKS = Neural_networks
EXEC_XOR = XOR

# Default rule (all)
all: $(EXEC_NEURAL_NETWORKS) $(EXEC_XOR)

# Compile Neural_networks
$(EXEC_NEURAL_NETWORKS): Neural_networks.c
	$(CC) $(CFLAGS) -o $(EXEC_NEURAL_NETWORKS) Neural_networks.c

# Compile XOR
$(EXEC_XOR): XOR.c
	$(CC) $(CFLAGS) -o $(EXEC_XOR) XOR.c

# Clean up executable and debug files
clean:
	rm -rf $(EXEC_NEURAL_NETWORKS) $(EXEC_XOR) Neural_networks.dSYM XOR.dSYM

