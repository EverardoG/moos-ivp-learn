#include "network.h"
#include <iostream>
#include <vector>

int main() {
    // Define weights for the network
    std::vector<double> weights = {
        // Weights for layer 1 (input to hidden)
        0.1, 0.2, 0.3, 0.4, 0.5, // Node 1
        0.6, 0.7, 0.8, 0.9, 1.0, // Node 2
        // Weights for layer 2 (hidden to output)
        1.1, 1.2, 1.3,           // Node 1
        1.4, 1.5, 1.6            // Node 2
    };

    // Define the structure of the network: 3 inputs, 2 hidden nodes, 2 outputs
    std::vector<int> structure = {3, 2, 2};

    // Create the neural network
    NeuralNetwork network(weights, structure);

    // Define input values
    std::vector<double> inputs = {0.5, 0.6, 0.7};

    // Perform a forward pass
    std::vector<double> outputs = network.forward(inputs);

    // Print the outputs
    std::cout << "Network outputs:" << std::endl;
    for (double output : outputs) {
        std::cout << output << std::endl;
    }

    return 0;
}
