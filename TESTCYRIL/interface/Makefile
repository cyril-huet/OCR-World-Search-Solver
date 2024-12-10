# Variables
CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

SRC = Interface.c
OUTPUT = interface

# Répertoires et binaires externes
LINK_PREPROCESS_DIR = ../link_pretraitement
LINK_PREPROCESS_BIN = $(LINK_PREPROCESS_DIR)/link_pretraitement
EXECUTABLE_DIR = ../extraction
EXECUTABLE_BIN = $(EXECUTABLE_DIR)/executable
SOLVER_DIR = ../solver
SOLVER_BINARIES = $(SOLVER_DIR)/solver_grid $(SOLVER_DIR)/solver_list $(SOLVER_DIR)/contract

# Fichiers indicateurs
PREPROCESS_FLAG = .preprocess_ready
SOLVER_FLAG = .solver_ready

# Cibles principales
.PHONY: all clean preprocess solver

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(LINK_PREPROCESS_BIN):
	$(MAKE) -C $(LINK_PREPROCESS_DIR) -lSDL2 -lSDL2_image -lm

$(EXECUTABLE_BIN):
	$(MAKE) -C $(EXECUTABLE_DIR) -lSDL2 -lSDL2_image -lm

$(SOLVER_BINARIES):
	$(MAKE) -C $(SOLVER_DIR)

# Étape conditionnelle pour le prétraitement
preprocess: $(LINK_PREPROCESS_BIN)
	@touch $(PREPROCESS_FLAG)

$(LINK_PREPROCESS_BIN): $(PREPROCESS_FLAG)
	$(MAKE) -C $(LINK_PREPROCESS_DIR) -lSDL2 -lSDL2_image -lm

# Étape conditionnelle pour le solver
solver: $(EXECUTABLE_BIN)
	@touch $(SOLVER_FLAG)

$(EXECUTABLE_BIN): $(SOLVER_FLAG)
	$(MAKE) -C $(EXECUTABLE_DIR) -lSDL2 -lSDL2_image -lm

clean:
	rm -f $(OUTPUT) $(PREPROCESS_FLAG) $(SOLVER_FLAG)
	$(MAKE) -C $(LINK_PREPROCESS_DIR) clean
	$(MAKE) -C $(SOLVER_DIR) clean
	$(MAKE) -C $(EXECUTABLE_DIR) clean

