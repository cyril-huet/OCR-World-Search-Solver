#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define INPUT_NODES 16       // Number of inputs (e.g., 4x4 pixels)
#define HIDDEN_NODES 10      // Hidden neurons
#define OUTPUT_NODES 26      // Output neurons
#define LEARNING_RATE 0.1    // Learning rate
#define EPOCHS 10000         // Number of iterations

// Activation function (Sigmoid)
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Derivative of the activation function
double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

// Neural network structure
typedef struct {
    double input_hidden_weights[INPUT_NODES][HIDDEN_NODES];
    double hidden_output_weights[HIDDEN_NODES][OUTPUT_NODES];
    double hidden_layer[HIDDEN_NODES];
    double output_layer[OUTPUT_NODES];
} NeuralNetwork;

// Initialize weights randomly
void initialize_network(NeuralNetwork *nn) {
    for (int i = 0; i < INPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            nn->input_hidden_weights[i][j] = 
                ((double)rand() / RAND_MAX) * 2 - 1; // Values between -1 and 1
        }
    }
    for (int i = 0; i < HIDDEN_NODES; i++) {
        for (int j = 0; j < OUTPUT_NODES; j++) {
            nn->hidden_output_weights[i][j] = 
                ((double)rand() / RAND_MAX) * 2 - 1; // Values between -1 and 1
        }
    }
}

// Train the network
void train(NeuralNetwork *nn, double inputs[][INPUT_NODES], 
           double outputs[][OUTPUT_NODES], int num_patterns) {
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        for (int p = 0; p < num_patterns; p++) {
            // Forward propagation
            for (int i = 0; i < HIDDEN_NODES; i++) {
                nn->hidden_layer[i] = 0.0;
                for (int j = 0; j < INPUT_NODES; j++) {
                    nn->hidden_layer[i] += 
                        inputs[p][j] * nn->input_hidden_weights[j][i];
                }
                nn->hidden_layer[i] = sigmoid(nn->hidden_layer[i]);
            }
            for (int i = 0; i < OUTPUT_NODES; i++) {
                nn->output_layer[i] = 0.0;
                for (int j = 0; j < HIDDEN_NODES; j++) {
                    nn->output_layer[i] += 
                        nn->hidden_layer[j] * nn->hidden_output_weights[j][i];
                }
                nn->output_layer[i] = sigmoid(nn->output_layer[i]);
            }

            // Calculate error
            double output_error[OUTPUT_NODES];
            for (int i = 0; i < OUTPUT_NODES; i++) {
                output_error[i] = outputs[p][i] - nn->output_layer[i];
            }

            // Backpropagation
            for (int i = 0; i < OUTPUT_NODES; i++) {
                double delta = output_error[i] * 
                    sigmoid_derivative(nn->output_layer[i]);
                for (int j = 0; j < HIDDEN_NODES; j++) {
                    nn->hidden_output_weights[j][i] += 
                        LEARNING_RATE * delta * nn->hidden_layer[j];
                }
            }
            for (int i = 0; i < HIDDEN_NODES; i++) {
                double hidden_error = 0.0;
                for (int j = 0; j < OUTPUT_NODES; j++) {
                    hidden_error += output_error[j] * 
                        nn->hidden_output_weights[i][j];
                }
                hidden_error *= sigmoid_derivative(nn->hidden_layer[i]);
                for (int j = 0; j < INPUT_NODES; j++) {
                    nn->input_hidden_weights[j][i] += 
                        LEARNING_RATE * hidden_error * inputs[p][j];
                }
            }
        }
    }
}

// Function to read a binary image from a file
int read_binary_image(const char *filename, double input[INPUT_NODES]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    for (int i = 0; i < INPUT_NODES; i++) {
        if (fscanf(file, "%lf", &input[i]) != 1) {
            fprintf(stderr, "Error reading file (incorrect format)\n");
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}

// Test the network
void test(NeuralNetwork *nn, double input[INPUT_NODES]) {
    // Forward propagation for hidden layer
    for (int i = 0; i < HIDDEN_NODES; i++) {
        nn->hidden_layer[i] = 0.0;
        for (int j = 0; j < INPUT_NODES; j++) {
            nn->hidden_layer[i] += input[j] * 
                nn->input_hidden_weights[j][i];
        }
        nn->hidden_layer[i] = sigmoid(nn->hidden_layer[i]);
    }

    // Forward propagation for output layer
    for (int i = 0; i < OUTPUT_NODES; i++) {
        nn->output_layer[i] = 0.0;
        for (int j = 0; j < HIDDEN_NODES; j++) {
            nn->output_layer[i] += nn->hidden_layer[j] * 
                nn->hidden_output_weights[j][i];
        }
        nn->output_layer[i] = sigmoid(nn->output_layer[i]);
    }

    int predicted_letter = -1;
    double max_output = 0.0;

    for (int i = 0; i < OUTPUT_NODES; i++) {
        if (nn->output_layer[i] > max_output) {
            max_output = nn->output_layer[i];
            predicted_letter = i;
        }
    }
    printf("Network output: %f\n", max_output);

    if (predicted_letter != -1 && max_output > 0.5) {
        printf("It's a %c!\n", 'A' + predicted_letter);
    } else {
        printf("Not recognized\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary_image_file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    srand((unsigned int)time(NULL));
    NeuralNetwork nn;
    initialize_network(&nn);

    // Define training data
    // Binary inputs for letters 'A', 'B', 'C', 'D'

double inputs[][INPUT_NODES] = {
    // 'A'
    {0, 1, 1, 0,
     1, 0, 0, 1,
     1, 1, 1, 1,
     1, 0, 0, 1},

    // 'B'
    {1, 1, 1, 0,
     1, 0, 1, 0,
     1, 1, 1, 0,
     1, 0, 1, 0},

    // 'C'
    {0, 1, 1, 1,
     1, 0, 0, 0,
     1, 0, 0, 0,
     0, 1, 1, 1},

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

// outputs
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
    
    // training
    train(&nn, inputs, outputs, num_patterns);

    // get the image to train 
    double input[INPUT_NODES];
    if (read_binary_image(filename, input)) {
        test(&nn, input);
    }

    return 0;
}
