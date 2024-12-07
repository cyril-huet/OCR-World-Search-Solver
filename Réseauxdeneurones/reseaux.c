#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

 
#define IMAGE_SIZE 28          // Taille de l'image (28x28 pixels)
#define KERNEL_SIZE 3          // Taille du filtre de convolution (3x3)
#define CONV_OUTPUT_SIZE 26    // Taille après convolution (28 - 3 + 1)
#define POOL_SIZE 2            // Taille du pooling (2x2)
#define POOL_OUTPUT_SIZE 13    // Taille après max pooling (26 / 2)

#define INPUT_NODES (POOL_OUTPUT_SIZE * POOL_OUTPUT_SIZE) // Taille après pooling
#define HIDDEN_NODES 128      // Nombre de neurones cachés
#define OUTPUT_NODES 26       // Nombre de classes (lettres de A à Z)
#define LEARNING_RATE 0.3     // Taux d'apprentissage
#define EPOCHS 500            // Nombre d'itérations

// Fonction d'activation (sigmoïde)
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Dérivée de la fonction d'activation
double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

// Convolution 2D
void convolution(double input[IMAGE_SIZE][IMAGE_SIZE], double kernel[KERNEL_SIZE][KERNEL_SIZE], double output[CONV_OUTPUT_SIZE][CONV_OUTPUT_SIZE]) {
    for (int i = 0; i < CONV_OUTPUT_SIZE; i++) {
        for (int j = 0; j < CONV_OUTPUT_SIZE; j++) {
            double sum = 0.0;
            for (int ki = 0; ki < KERNEL_SIZE; ki++) {
                for (int kj = 0; kj < KERNEL_SIZE; kj++) {
                    sum += input[i + ki][j + kj] * kernel[ki][kj];
                }
            }
            output[i][j] = sum;
        }
    }
}

// Max pooling 2D
void max_pooling(double input[CONV_OUTPUT_SIZE][CONV_OUTPUT_SIZE], double output[POOL_OUTPUT_SIZE][POOL_OUTPUT_SIZE]) {
    for (int i = 0; i < POOL_OUTPUT_SIZE; i++) {
        for (int j = 0; j < POOL_OUTPUT_SIZE; j++) {
            double max_val = -1.0;
            for (int pi = 0; pi < POOL_SIZE; pi++) {
                for (int pj = 0; pj < POOL_SIZE; pj++) {
                    double val = input[i * POOL_SIZE + pi][j * POOL_SIZE + pj];
                    if (val > max_val) {
                        max_val = val;
                    }
                }
            }
            output[i][j] = max_val;
        }
    }
}

// Structure du réseau neuronal
typedef struct {
    double conv_kernel[KERNEL_SIZE][KERNEL_SIZE];                 // Filtre de convolution
    double conv_output[CONV_OUTPUT_SIZE][CONV_OUTPUT_SIZE];       // Sortie de la convolution
    double pooled_output[POOL_OUTPUT_SIZE][POOL_OUTPUT_SIZE];     // Sortie du pooling
    double input_hidden_weights[INPUT_NODES][HIDDEN_NODES];       // Poids entre entrée et couche cachée
    double hidden_output_weights[HIDDEN_NODES][OUTPUT_NODES];     // Poids entre couche cachée et sortie
    double hidden_bias[HIDDEN_NODES];
    double output_bias[OUTPUT_NODES];
    double hidden_layer[HIDDEN_NODES];
    double output_layer[OUTPUT_NODES];
} NeuralNetwork;

// Initialisation aléatoire des poids et biais
void initialize_network(NeuralNetwork *nn) {
    // Initialisation du filtre de convolution
    for (int i = 0; i < KERNEL_SIZE; i++) {
        for (int j = 0; j < KERNEL_SIZE; j++) {
            nn->conv_kernel[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
    }
    // Initialisation des poids et biais
    for (int i = 0; i < INPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            nn->input_hidden_weights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
    }
    for (int i = 0; i < HIDDEN_NODES; i++) {
        for (int j = 0; j < OUTPUT_NODES; j++) {
            nn->hidden_output_weights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
        nn->hidden_bias[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    }
    for (int i = 0; i < OUTPUT_NODES; i++) {
        nn->output_bias[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    }
}

// Lecture d'une image PGM
int load_pgm(const char *filepath, double image[IMAGE_SIZE][IMAGE_SIZE]) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier : %s\n", filepath);
        return 0;
    }

    char format[3];
    int width, height, max_value;

    // Lire l'en-tête PGM
    if (fscanf(file, "%2s", format) != 1 || strcmp(format, "P5") != 0) {
        fprintf(stderr, "Format PGM non supporté : %s\n", filepath);
        fclose(file);
        return 0;
    }
    fscanf(file, "%d %d %d", &width, &height, &max_value);
    fgetc(file);  // Ignorer un caractère pour atteindre les données binaires

    if (width != IMAGE_SIZE || height != IMAGE_SIZE) {
        fprintf(stderr, "Taille incorrecte pour %s : attendu %dx%d, obtenu %dx%d\n", filepath, IMAGE_SIZE, IMAGE_SIZE, width, height);
        fclose(file);
        return 0;
    }

    // Charger les données
    unsigned char buffer[IMAGE_SIZE][IMAGE_SIZE];
    fread(buffer, sizeof(unsigned char), IMAGE_SIZE * IMAGE_SIZE, file);

    // Normaliser les pixels
    for (int i = 0; i < IMAGE_SIZE; i++) {
        for (int j = 0; j < IMAGE_SIZE; j++) {
            image[i][j] = buffer[i][j] / 255.0;
        }
    }

    fclose(file);
    return 1;
}



// Sauvegarder les poids et biais
void save_weights(NeuralNetwork *nn, const char *filepath) {
    FILE *file = fopen(filepath, "wb");
    if (!file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier pour la sauvegarde : %s\n", filepath);
        return;
    }

    // Sauvegarder le filtre de convolution
    fwrite(nn->conv_kernel, sizeof(double), KERNEL_SIZE * KERNEL_SIZE, file);

    // Sauvegarder les poids et biais entre l'entrée et la couche cachée
    fwrite(nn->input_hidden_weights, sizeof(double), INPUT_NODES * HIDDEN_NODES, file);
    fwrite(nn->hidden_bias, sizeof(double), HIDDEN_NODES, file);

    // Sauvegarder les poids et biais entre la couche cachée et la sortie
    fwrite(nn->hidden_output_weights, sizeof(double), HIDDEN_NODES * OUTPUT_NODES, file);
    fwrite(nn->output_bias, sizeof(double), OUTPUT_NODES, file);

    fclose(file);
    printf("Poids et biais sauvegardés dans le fichier : %s\n", filepath);
}

// Charger les poids et biais
void load_weights(NeuralNetwork *nn, const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier pour le chargement : %s\n", filepath);
        return;
    }

    // Charger le filtre de convolution
    fread(nn->conv_kernel, sizeof(double), KERNEL_SIZE * KERNEL_SIZE, file);

    // Charger les poids et biais entre l'entrée et la couche cachée
    fread(nn->input_hidden_weights, sizeof(double), INPUT_NODES * HIDDEN_NODES, file);
    fread(nn->hidden_bias, sizeof(double), HIDDEN_NODES, file);

    // Charger les poids et biais entre la couche cachée et la sortie
    fread(nn->hidden_output_weights, sizeof(double), HIDDEN_NODES * OUTPUT_NODES, file);
    fread(nn->output_bias, sizeof(double), OUTPUT_NODES, file);

    fclose(file);
    printf("Poids et biais chargés depuis le fichier : %s\n", filepath);
}


int load_dataset(const char *dataset_path, double dataset[][IMAGE_SIZE][IMAGE_SIZE], double labels[][OUTPUT_NODES]) {
    DIR *dir;
    struct dirent *entry;
    int num_samples = 0;

    if ((dir = opendir(dataset_path)) == NULL) {
        perror("Erreur lors de l'ouverture du dossier du dataset");
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
            char letter = entry->d_name[0];
            int label_index = letter - 'A';

            char subfolder[512]; // Taille ajustée
            if (snprintf(subfolder, sizeof(subfolder), "%s/%s", dataset_path, entry->d_name) >= sizeof(subfolder)) {
                fprintf(stderr, "Erreur : chemin sous-dossier trop long pour %s/%s\n", dataset_path, entry->d_name);
                continue;
            }

            DIR *subdir;
            struct dirent *subentry;
            if ((subdir = opendir(subfolder)) == NULL) {
                continue;
            }

            while ((subentry = readdir(subdir)) != NULL) {
                if (strstr(subentry->d_name, ".pgm")) {
                    char filepath[1024]; // Augmenté à 1024
                    if (snprintf(filepath, sizeof(filepath), "%s/%s", subfolder, subentry->d_name) >= sizeof(filepath)) {
                        fprintf(stderr, "Erreur : chemin de fichier trop long pour %s/%s\n", subfolder, subentry->d_name);
                        continue;
                    }

                    if (load_pgm(filepath, dataset[num_samples])) {
                        for (int i = 0; i < OUTPUT_NODES; i++) {
                            labels[num_samples][i] = (i == label_index) ? 1.0 : 0.0;
                        }
                        num_samples++;
                        if (num_samples >= 3489) { // Limite pour cet exemple
                            break;
                        }
                    }
                }
            }
            closedir(subdir);
        }
    }
    closedir(dir);

    return num_samples;
}

// Propagation avant
void forward_propagation(NeuralNetwork *nn, double input[IMAGE_SIZE][IMAGE_SIZE]) {
    // Étape 1 : Convolution
    convolution(input, nn->conv_kernel, nn->conv_output);

    // Étape 2 : Max pooling
    max_pooling(nn->conv_output, nn->pooled_output);

    // Étape 3 : Aplatir les données
    double flattened_input[INPUT_NODES];
    for (int i = 0; i < POOL_OUTPUT_SIZE; i++) {
        for (int j = 0; j < POOL_OUTPUT_SIZE; j++) {
            flattened_input[i * POOL_OUTPUT_SIZE + j] = nn->pooled_output[i][j];
        }
    }

    // Étape 4 : Couche cachée
    for (int i = 0; i < HIDDEN_NODES; i++) {
        nn->hidden_layer[i] = nn->hidden_bias[i];
        for (int j = 0; j < INPUT_NODES; j++) {
            nn->hidden_layer[i] += flattened_input[j] * nn->input_hidden_weights[j][i];
        }
        nn->hidden_layer[i] = sigmoid(nn->hidden_layer[i]);
    }

    // Étape 5 : Couche de sortie
    for (int i = 0; i < OUTPUT_NODES; i++) {
        nn->output_layer[i] = nn->output_bias[i];
        for (int j = 0; j < HIDDEN_NODES; j++) {
            nn->output_layer[i] += nn->hidden_layer[j] * nn->hidden_output_weights[j][i];
        }
        nn->output_layer[i] = sigmoid(nn->output_layer[i]);
    }
}


// Entraîner le réseau
void train(NeuralNetwork *nn, double inputs[][IMAGE_SIZE][IMAGE_SIZE], double outputs[][OUTPUT_NODES], int num_samples) {
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        for (int sample = 0; sample < num_samples; sample++) {
            // Propagation avant
            forward_propagation(nn, inputs[sample]);

            // Calcul de l'erreur
            double output_error[OUTPUT_NODES];
            for (int i = 0; i < OUTPUT_NODES; i++) {
                output_error[i] = outputs[sample][i] - nn->output_layer[i];
            }

            // Rétropropagation
            for (int i = 0; i < OUTPUT_NODES; i++) {
                double delta_output = output_error[i] * sigmoid_derivative(nn->output_layer[i]);
                for (int j = 0; j < HIDDEN_NODES; j++) {
                    nn->hidden_output_weights[j][i] += LEARNING_RATE * delta_output * nn->hidden_layer[j];
                }
                nn->output_bias[i] += LEARNING_RATE * delta_output;
            }

            for (int i = 0; i < HIDDEN_NODES; i++) {
                double hidden_error = 0.0;
                for (int j = 0; j < OUTPUT_NODES; j++) {
                    hidden_error += output_error[j] * nn->hidden_output_weights[i][j];
                }
                hidden_error *= sigmoid_derivative(nn->hidden_layer[i]);
                for (int j = 0; j < INPUT_NODES; j++) {
                    nn->input_hidden_weights[j][i] += LEARNING_RATE * hidden_error * nn->pooled_output[j / POOL_OUTPUT_SIZE][j % POOL_OUTPUT_SIZE];
                }
                nn->hidden_bias[i] += LEARNING_RATE * hidden_error;
            }
		double error_sum = 0.0;
for (int i = 0; i < OUTPUT_NODES; i++) {
    error_sum += fabs(outputs[sample][i] - nn->output_layer[i]);
}

        }
        if (epoch % 10 == 0) {
            printf("Époque %d terminée\n", epoch);
        }
    }
}

// Test du réseau
void test(NeuralNetwork *nn, double input[IMAGE_SIZE][IMAGE_SIZE]) {
    // Effectuer la propagation avant
    forward_propagation(nn, input);

    // Déterminer la classe prédite
    int predicted_class = 0;
    double max_probability = nn->output_layer[0];
    for (int i = 1; i < OUTPUT_NODES; i++) {
        if (nn->output_layer[i] > max_probability) {
            max_probability = nn->output_layer[i];
            predicted_class = i;
        }
    }

    // Afficher les résultats
    printf("Classe prédite : %c avec probabilité %.2f\n", 'A' + predicted_class, max_probability);

    // Optionnel : afficher toutes les probabilités pour les autres classes
    for (int i = 0; i < OUTPUT_NODES; i++) {
        printf("Classe %c : %.2f\n", 'A' + i, nn->output_layer[i]);
    }
}


// Fonction pour tester un dossier d'images
void test_directory(NeuralNetwork *nn, const char *directory_path) {
    DIR *dir = opendir(directory_path);
    if (!dir) {
        perror("Erreur : impossible d'ouvrir le dossier");
        return;
    }

    FILE *output_file = fopen("matrice.txt", "w");
    if (!output_file) {
        perror("Erreur : impossible d'ouvrir le fichier matrice.txt");
        closedir(dir);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".pgm")) {
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", directory_path, entry->d_name);

            double test_image[IMAGE_SIZE][IMAGE_SIZE];
            if (load_pgm(filepath, test_image)) {
                // Reconnaître la lettre
                test(nn, test_image);

                // Trouver la classe prédite
                int predicted_class = 0;
                double max_probability = nn->output_layer[0];
                for (int i = 1; i < OUTPUT_NODES; i++) {
                    if (nn->output_layer[i] > max_probability) {
                        max_probability = nn->output_layer[i];
                        predicted_class = i;
                    }
                }

                // Écrire la lettre dans le fichier texte
                fprintf(output_file, "%s: %c\n", entry->d_name, 'A' + predicted_class);
            } else {
                fprintf(stderr, "Erreur : impossible de charger %s\n", filepath);
            }
        }
    }

    fclose(output_file);
    closedir(dir);
    printf("Résultats écrits dans le fichier matrice.txt\n");
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <weights_file> <dataset_or_image_path> [test_directory]\n", argv[0]);
        return 1;
    }

    srand((unsigned int)time(NULL));
    NeuralNetwork nn;

    // Charger les poids
    if (fopen(argv[1], "rb")) {
        load_weights(&nn, argv[1]);
    } else {
        fprintf(stderr, "Erreur : fichier de poids introuvable : %s\n", argv[1]);
        return 1;
    }

    // Vérifier si le deuxième argument est un répertoire ou un fichier
    struct stat path_stat;
    if (stat(argv[2], &path_stat) != 0) {
        perror("Erreur : chemin non valide");
        return 1;
    }

    if (S_ISDIR(path_stat.st_mode)) {
        // Si c'est un dossier, entraîner le réseau
        int num_samples = 3489; // Nombre maximal d'échantillons
        double (*dataset)[IMAGE_SIZE][IMAGE_SIZE] = malloc(num_samples * sizeof(*dataset));
        double (*labels)[OUTPUT_NODES] = malloc(num_samples * sizeof(*labels));
        if (!dataset || !labels) {
            fprintf(stderr, "Erreur : allocation mémoire échouée.\n");
            return 1;
        }

        num_samples = load_dataset(argv[2], dataset, labels);
        if (num_samples == 0) {
            fprintf(stderr, "Erreur : chargement du dataset échoué.\n");
            return 1;
        }

        train(&nn, dataset, labels, num_samples);
        save_weights(&nn, argv[1]);

        free(dataset);
        free(labels);
    } else if (S_ISREG(path_stat.st_mode)) {
        // Si c'est un fichier, effectuer une reconnaissance
        double test_image[IMAGE_SIZE][IMAGE_SIZE];
        if (load_pgm(argv[2], test_image)) {
            test(&nn, test_image);
        } else {
            fprintf(stderr, "Erreur : impossible de charger l'image de test : %s\n", argv[2]);
            return 1;
        }
    } else {
        fprintf(stderr, "Erreur : chemin non valide\n");
        return 1;
    }

    // Si un troisième argument est fourni, tester un dossier d'images
    if (argc > 3) {
        test_directory(&nn, argv[3]);
    }

    return 0;
}
