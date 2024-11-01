#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Constants for defining the neural network structure and learning parameters
#define INPUT_NEURONS 2     // Number of input neurons (for XOR: 2 inputs)
#define HIDDEN_NEURONS 4    // Number of neurons in the hidden layer
#define OUTPUT_NEURONS 1    // Number of output neurons (for XOR: 1 output)
#define LEARNING_RATE 0.01  // Learning rate for weight adjustments
#define TRAINING_EPOCHS 50000 // Number of training iterations (epochs)

// Function to round a floating-point number to the nearest integer
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

// Sigmoid activation function to introduce non-linearity
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Derivative of the sigmoid function for backpropagation
double sigmoidDerivative(double x) {
    return x * (1.0 - x);
}

// Function for the forward pass: calculates outputs for hidden and output layers
void forwardPass(double inputs[], double hiddenLayer[], double outputLayer[], double inputToHiddenWeights[INPUT_NEURONS][HIDDEN_NEURONS], double hiddenToOutputWeights[HIDDEN_NEURONS][OUTPUT_NEURONS]) {
    // Calculate hidden layer activations
    for (int i = 0; i < HIDDEN_NEURONS; i++) {
        hiddenLayer[i] = 0.0;
        for (int j = 0; j < INPUT_NEURONS; j++) {
            hiddenLayer[i] += inputs[j] * inputToHiddenWeights[j][i]; // Sum weighted inputs
        }
        hiddenLayer[i] = sigmoid(hiddenLayer[i]); // Apply activation function
    }

    // Calculate output layer activations
    for (int i = 0; i < OUTPUT_NEURONS; i++) {
        outputLayer[i] = 0.0;
        for (int j = 0; j < HIDDEN_NEURONS; j++) {
            outputLayer[i] += hiddenLayer[j] * hiddenToOutputWeights[j][i]; // Sum hidden outputs
        }
        outputLayer[i] = sigmoid(outputLayer[i]); // Apply activation function
    }
}

// Function for the backward pass: updates weights based on error
void backwardPass(double inputs[], double hiddenLayer[], double outputLayer[], double expectedOutput[], double inputToHiddenWeights[INPUT_NEURONS][HIDDEN_NEURONS], double hiddenToOutputWeights[HIDDEN_NEURONS][OUTPUT_NEURONS]) {
    double outputError[OUTPUT_NEURONS]; // Array to store output layer error
    double hiddenError[HIDDEN_NEURONS]; // Array to store hidden layer error

    // Calculate error for output layer and adjust hidden-to-output weights
    for (int i = 0; i < OUTPUT_NEURONS; i++) {
        outputError[i] = expectedOutput[i] - outputLayer[i]; // Difference between expected and actual output
        for (int j = 0; j < HIDDEN_NEURONS; j++) {
            // Update weights based on output error and learning rate
            hiddenToOutputWeights[j][i] += LEARNING_RATE * outputError[i] * sigmoidDerivative(outputLayer[i]) * hiddenLayer[j];
        }
    }

    // Calculate error for hidden layer and adjust input-to-hidden weights
    for (int i = 0; i < HIDDEN_NEURONS; i++) {
        hiddenError[i] = 0.0;
        for (int j = 0; j < OUTPUT_NEURONS; j++) {
            hiddenError[i] += outputError[j] * sigmoidDerivative(outputLayer[j]) * hiddenToOutputWeights[i][j];
        }
        for (int j = 0; j < INPUT_NEURONS; j++) {
            // Update weights based on hidden error and learning rate
            inputToHiddenWeights[j][i] += LEARNING_RATE * hiddenError[i] * sigmoidDerivative(hiddenLayer[i]) * inputs[j];
        }
    }
}

int main() {
    // Define the input data for XOR and the target output
    double inputs[4][INPUT_NEURONS] = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
    };

    double expectedOutputs[4][OUTPUT_NEURONS] = {
        {1}, // XOR(0,0) = 1
        {0}, // XOR(0,1) = 0
        {0}, // XOR(1,0) = 0
        {1}  // XOR(1,1) = 1
    };

    // Initialize weights with random values between -1 and 1
    double inputToHiddenWeights[INPUT_NEURONS][HIDDEN_NEURONS];
    double hiddenToOutputWeights[HIDDEN_NEURONS][OUTPUT_NEURONS];

    for (int i = 0; i < INPUT_NEURONS; i++) {
        for (int j = 0; j < HIDDEN_NEURONS; j++) {
            inputToHiddenWeights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
    }

    for (int i = 0; i < HIDDEN_NEURONS; i++) {
        for (int j = 0; j < OUTPUT_NEURONS; j++) {
            hiddenToOutputWeights[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
    }

    // Training loop to adjust weights over multiple epochs
    for (int epoch = 0; epoch < TRAINING_EPOCHS; epoch++) {
        for (int i = 0; i < 4; i++) {
            double hiddenLayer[HIDDEN_NEURONS];
            double outputLayer[OUTPUT_NEURONS];
            forwardPass(inputs[i], hiddenLayer, outputLayer, inputToHiddenWeights, hiddenToOutputWeights);
            backwardPass(inputs[i], hiddenLayer, outputLayer, expectedOutputs[i], inputToHiddenWeights, hiddenToOutputWeights);
        }
    }

    // Test the trained neural network
    printf("Displaying XOR results with rounded values:\n");
    for (int i = 0; i < 4; i++) {
        double hiddenLayer[HIDDEN_NEURONS];
        double outputLayer[OUTPUT_NEURONS];
        forwardPass(inputs[i], hiddenLayer, outputLayer, inputToHiddenWeights, hiddenToOutputWeights);
        printf("Input: %d %d, Output: %d\n", (int)inputs[i][0], (int)inputs[i][1], roundToNearestInt(outputLayer[0]));
    }

    printf("\nDisplaying XOR results with exact values:\n");
    for (int i = 0; i < 4; i++) {
        double hiddenLayer[HIDDEN_NEURONS];
        double outputLayer[OUTPUT_NEURONS];
        forwardPass(inputs[i], hiddenLayer, outputLayer, inputToHiddenWeights, hiddenToOutputWeights);
        printf("Input: %d %d, Output: %f\n", (int)inputs[i][0], (int)inputs[i][1], outputLayer[0]);
    }

    return 0;
}

