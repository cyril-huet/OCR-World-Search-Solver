#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define INPUT_NODES 16       // Nombre d'entrées (par exemple, 4x4 pixels)
#define HIDDEN_NODES 10      // Neurones cachés
#define OUTPUT_NODES 26       // Neurones de sortie
#define LEARNING_RATE 0.1    // Taux d'apprentissage
#define EPOCHS 10000         // Nombre d'itérations

// Fonction d'activation (Sigmoïde)
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Dérivée de la fonction d'activation
double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

// Structure du réseau
typedef struct {
    double input_hidden_weights[INPUT_NODES][HIDDEN_NODES];
    double hidden_output_weights[HIDDEN_NODES][OUTPUT_NODES];
    double hidden_layer[HIDDEN_NODES];
    double output_layer[OUTPUT_NODES];
} NeuralNetwork;

// Initialisation des poids aléatoires
void initialize_network(NeuralNetwork *nn) {
    for (int i = 0; i < INPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            nn->input_hidden_weights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1; // Valeurs entre -1 et 1
        }
    }
    for (int i = 0; i < HIDDEN_NODES; i++) {
        for (int j = 0; j < OUTPUT_NODES; j++) {
            nn->hidden_output_weights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1; // Valeurs entre -1 et 1
        }
    }
}

// Entraînement du réseau
void train(NeuralNetwork *nn, double inputs[][INPUT_NODES], double outputs[][OUTPUT_NODES], int num_patterns) {
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        for (int p = 0; p < num_patterns; p++) {
            // Propagation avant
            for (int i = 0; i < HIDDEN_NODES; i++) {
                nn->hidden_layer[i] = 0.0;
                for (int j = 0; j < INPUT_NODES; j++) {
                    nn->hidden_layer[i] += inputs[p][j] * nn->input_hidden_weights[j][i];
                }
                nn->hidden_layer[i] = sigmoid(nn->hidden_layer[i]);
            }
            for (int i = 0; i < OUTPUT_NODES; i++) {
                nn->output_layer[i] = 0.0;
                for (int j = 0; j < HIDDEN_NODES; j++) {
                    nn->output_layer[i] += nn->hidden_layer[j] * nn->hidden_output_weights[j][i];
                }
                nn->output_layer[i] = sigmoid(nn->output_layer[i]);
            }

            // Calcul de l'erreur
            double output_error[OUTPUT_NODES];
            for (int i = 0; i < OUTPUT_NODES; i++) {
                output_error[i] = outputs[p][i] - nn->output_layer[i];
            }

            // Propagation arrière
            for (int i = 0; i < OUTPUT_NODES; i++) {
                double delta = output_error[i] * sigmoid_derivative(nn->output_layer[i]);
                for (int j = 0; j < HIDDEN_NODES; j++) {
                    nn->hidden_output_weights[j][i] += LEARNING_RATE * delta * nn->hidden_layer[j];
                }
            }
            for (int i = 0; i < HIDDEN_NODES; i++) {
                double hidden_error = 0.0;
                for (int j = 0; j < OUTPUT_NODES; j++) {
                    hidden_error += output_error[j] * nn->hidden_output_weights[i][j];
                }
                hidden_error *= sigmoid_derivative(nn->hidden_layer[i]);
                for (int j = 0; j < INPUT_NODES; j++) {
                    nn->input_hidden_weights[j][i] += LEARNING_RATE * hidden_error * inputs[p][j];
                }
            }
        }
    }
}

// Fonction pour lire une image binarisée depuis un fichier
int lire_image_binaire(const char *fichier_image, double input[INPUT_NODES]) {
    FILE *file = fopen(fichier_image, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 0;
    }

    for (int i = 0; i < INPUT_NODES; i++) {
        if (fscanf(file, "%lf", &input[i]) != 1) {
            fprintf(stderr, "Erreur de lecture dans le fichier (format incorrect)\n");
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}

// Test du réseau
void test(NeuralNetwork *nn, double input[INPUT_NODES]) {
    // Propagation avant pour la couche cachée
    for (int i = 0; i < HIDDEN_NODES; i++) {
        nn->hidden_layer[i] = 0.0;
        for (int j = 0; j < INPUT_NODES; j++) {
            nn->hidden_layer[i] += input[j] * nn->input_hidden_weights[j][i];
        }
        nn->hidden_layer[i] = sigmoid(nn->hidden_layer[i]);
    }

    // Propagation avant pour la couche de sortie
    for (int i = 0; i < OUTPUT_NODES; i++) {
        nn->output_layer[i] = 0.0;
        for (int j = 0; j < HIDDEN_NODES; j++) {
            nn->output_layer[i] += nn->hidden_layer[j] * nn->hidden_output_weights[j][i];
        }
        nn->output_layer[i] = sigmoid(nn->output_layer[i]);
    }

    int predicted_letter = -1;
	double max_output = 0.0;

	for(int i=0; i< OUTPUT_NODES; i++){
		if(nn->output_layer[i] > max_output){
			max_output = nn->output_layer[i];
			predicted_letter = i;
}
}
	printf("Sortie du réseau : %f\n", max_output);

if(predicted_letter != -1 && max_output > 0.5){
	printf("C'est un %c !\n", 'A' + predicted_letter);
}
else{
	printf("pas reconnue");}
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier_image_binaire>\n", argv[0]);
        return 1;
    }

    const char *fichier_image = argv[1];
    srand((unsigned int)time(NULL));
    NeuralNetwork nn;
    initialize_network(&nn);

    // Définir les données d'entraînement
    // Entrées binaires 4x4 pour les lettres 'A', 'B', 'C', 'D'
double inputs[][INPUT_NODES] = {
    // Représentation de 'A'
    {0, 1, 1, 0,
     1, 0, 0, 1,
     1, 1, 1, 1,
     1, 0, 0, 1},

    // Représentation de 'B'
    {1, 1, 1, 0,
     1, 0, 1, 0,
     1, 1, 1, 0,
     1, 0, 1, 0},

    // Représentation de 'C'
    {0, 1, 1, 1,
     1, 0, 0, 0,
     1, 0, 0, 0,
     0, 1, 1, 1},

    // Représentation de 'D'
    {1, 1, 1, 0,
     1, 0, 0, 1,
     1, 0, 0, 1,
     1, 1, 1, 0},
	{1, 1, 1, 1,
1, 0, 0, 0,
1, 1, 1, 0,
1, 1, 1, 1
},
{1, 1, 1, 1,
1, 0, 0, 0,
1, 1, 1, 0,
1, 0, 0, 0
},
{0, 1, 1, 1,
1, 0, 0, 0,
1, 0, 1, 1,
0, 1, 1, 1
},
{1, 0, 0, 1,
1, 0, 0, 1,
1, 1, 1, 1,
1, 0, 0, 1
},
{1, 1, 1, 1,
0, 1, 1, 0,
0, 1, 1, 0,
1, 1, 1, 1
},
{0, 0, 1, 1,
0, 0, 0, 1,
1, 0, 0, 1,
0, 1, 1, 0
},
{1, 0, 1, 0,
1, 1, 0, 0,
1, 0, 1, 0,
1, 0, 0, 1
},
{1, 0, 0, 0,
1, 0, 0, 0,
1, 0, 0, 0,
1, 1, 1, 1
},
{1, 0, 0, 1,
1, 1, 1, 1,
1, 0, 0, 1,
1, 0, 0, 1
},
{1, 0, 0, 1,
1, 1, 0, 1,
1, 0, 1, 1,
1, 0, 0, 1
},
{0, 1, 1, 0,
1, 0, 0, 1,
1, 0, 0, 1,
0, 1, 1, 0
},
{1, 1, 1, 0,
1, 0, 0, 1,
1, 1, 1, 0,
1, 0, 0, 0
},
{0, 1, 1, 0,
1, 0, 0, 1,
1, 0, 1, 1,
0, 1, 1, 1
},
{1, 1, 1, 0,
1, 0, 0, 1,
1, 1, 1, 0,
1, 0, 1, 1
},
{0, 1, 1, 1,
1, 0, 0, 0,
0, 1, 1, 0,
1, 1, 1, 0
},
{1, 1, 1, 1,
0, 1, 1, 0,
0, 1, 1, 0,
0, 1, 1, 0
},
{0, 0, 0, 0,
1, 0, 0, 1,
1, 0, 0, 1,
0, 1, 1, 0
},
{1, 0, 0, 1,
1, 0, 0, 1,
1, 0, 0, 1,
0, 1, 1, 0
},
{1, 0, 0, 1,
1, 0, 0, 1,
1, 1, 1, 1,
1, 0, 0, 1
},
{1, 0, 0, 1,
0, 1, 1, 0,
0, 1, 1, 0,
1, 0, 0, 1
},
{1, 0, 0, 1,
0, 1, 1, 0,
0, 1, 1, 0,
0, 1, 1, 0
},
{1, 1, 1, 1,
0, 0, 1, 0,
0, 1, 0, 0,
1, 1, 1, 1
}
};

// Sorties correspondantes
double outputs[][26] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};


    int num_patterns = sizeof(inputs) / sizeof(inputs[0]);
    
    // Entraînement du réseau
    train(&nn, inputs, outputs, num_patterns);

    // Lecture d'une image pour tester le réseau
    double input[INPUT_NODES];
    if (lire_image_binaire(fichier_image, input)) {
        test(&nn, input);
    }

    return 0;
}
