#include "network.h"
#include <iostream>
#include <vector>

int main() {
    // Define weights for the node (including bias weight)
    std::vector<double> weights = {1.0, 1.0, 1.0, 1.0}; // 3 inputs + 1 bias

    // Create a single node with Linear activation
    Node node(weights, Node::ActivationType::Tanh);

    // Define input values
    std::vector<double> inputs = {1.0, 1.0, 1.0};

    // Perform a forward pass
    double output = node.forward(inputs);

    // Print the output
    std::cout << "Node output: " << output << std::endl;

    return 0;
}
