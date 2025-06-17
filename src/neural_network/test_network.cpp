#include "network.h"
#include <iostream>
#include <vector>

int main() {
    // Define weights for the network
    std::vector<double> weights = {
        // Weights for layer 1 (input to hidden)
        1.0, 1.0, 1.0, 1.0, // Node 1
        1.0, 1.0, 1.0, 1.0, // Node 2
        // Weights for layer 2 (hidden to output)
        1.0, 1.0, 1.0,           // Node 1
        1.0, 1.0, 1.0            // Node 2
    };

    for (int i = 0; i < weights.size(); i++) {
        weights[i] = -weights[i];
    }

    // Define the structure of the network: 3 inputs, 2 hidden nodes, 2 outputs
    std::vector<int> structure = {3, 2, 2};

    // Define bounds for the output layer
    std::vector<std::vector<double>> bounds = {
        {-1.0, 1.0}, // Bounds for output node 1
        {-0.5, 0.5}  // Bounds for output node 2
    };

    // Create the neural network
    NeuralNetwork network(weights, structure, bounds);

    // Define input values
    std::vector<double> inputs = {1.0, 1.0, 1.0};

    // Perform a forward pass
    std::vector<double> outputs = network.forward(inputs);

    // Print the outputs
    std::cout << "Network outputs:" << std::endl;
    for (double output : outputs) {
        std::cout << output << std::endl;
    }

    return 0;
}
