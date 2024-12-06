#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Sigmoid activation function
double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

// Derivative of the sigmoid function
double sigmoid_derivative(double x) {
    return x * (1 - x);
}

typedef struct {
    double input[2];       // Input layer with two neurons
    double weights1[2][2]; // Weights from input to hidden layer
    double hidden[2];      // Hidden layer with two neurons
    double weights2[2];    // Weights from hidden layer to output
    double output;         // Output neuron
} NeuralNetwork;

// Function to round to the nearest integer
int roundToNearestInt(double number) {
    int integerPart = (int)number;
    double fractionalPart = number - integerPart;
    if (fractionalPart >= 0.5) {
        return integerPart + 1;
    } else if (fractionalPart <= -0.5) {
        return integerPart - 1;
    } else {
        return integerPart;
    }
}

// Initialize weights with random values
void initialize_weights(NeuralNetwork *nn) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            nn->weights1[i][j] = (double)rand() / RAND_MAX;
        }
        nn->weights2[i] = (double)rand() / RAND_MAX;
    }
}

// Forward propagation
void forward_propagate(NeuralNetwork *nn) {
    // Calculate hidden layer values
    for (int i = 0; i < 2; i++) {
        nn->hidden[i] = 0;
        for (int j = 0; j < 2; j++) {
            nn->hidden[i] += nn->input[j] * nn->weights1[j][i];
        }
        nn->hidden[i] = sigmoid(nn->hidden[i]); // Apply activation to hidden
    }

    // Calculate output layer value
    nn->output = 0;
    for (int i = 0; i < 2; i++) {
        nn->output += nn->hidden[i] * nn->weights2[i];
    }
    nn->output = sigmoid(nn->output); // Apply activation to output
}

// Training function (weight adjustment)
void train(NeuralNetwork *nn, double inputs[4][2], double expected_outputs[4], 
           int epochs, double learning_rate) {
    for (int e = 0; e < epochs; e++) {
        double total_error = 0;
        
        // Iterate
        for (int p = 0; p < 4; p++) {
            nn->input[0] = inputs[p][0];
            nn->input[1] = inputs[p][1];
            forward_propagate(nn);

            // Calculate error
            double error = expected_outputs[p] - nn->output;
            total_error += error * error;

            // Backpropagation to adjust weights
            double delta_output = error * sigmoid_derivative(nn->output);

            double delta_hidden[2];
            for (int i = 0; i < 2; i++) {
                delta_hidden[i] = delta_output * nn->weights2[i] * 
                                  sigmoid_derivative(nn->hidden[i]);
            }

            // Update output weights
            for (int i = 0; i < 2; i++) {
                nn->weights2[i] += learning_rate * delta_output * nn->hidden[i];
            }

            // Update input-to-hidden weights
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    nn->weights1[j][i] += learning_rate * delta_hidden[i] * 
                                          nn->input[j];
                }
            }
        }
        
        // Print average error per epoch
        if (e % 1000 == 0) {
            printf("Epoch %d, Average Error: %f\n", e, total_error / 4);
        }
    }
}

int main() {
    NeuralNetwork nn;
    initialize_weights(&nn);

    // Inputs for NON XOR
    double inputs[4][2] = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
    };
    // Expected outputs for NON XOR
    double expected_outputs[4] = {1, 0, 0, 1};

    // Train the network
    train(&nn, inputs, expected_outputs, 10000, 0.1);

    // Test the network after training
    for (int i = 0; i < 4; i++) {
        nn.input[0] = inputs[i][0];
        nn.input[1] = inputs[i][1];
        forward_propagate(&nn);
        printf("Input: %.0f, %.0f -> Output: %.2f (Expected: %.0f)\n", 
               nn.input[0], nn.input[1], nn.output, expected_outputs[i]);
    }

    return 0;
}

