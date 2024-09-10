# Définir les variables
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`
TARGET = interface
SRC = test.c

# Règle par défaut
all: $(TARGET)

# Règle de compilation
$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(LIBS)

# Règle pour lancer le programme
run: $(TARGET)
	./$(TARGET)

# Règle pour nettoyer les fichiers générés
clean:
	rm -f $(TARGET)
