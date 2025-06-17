#include "network.hpp"
#include <iostream>
#include <vector>

int main() {
    // Define weights for two nodes (including bias weights)
    std::vector<double> weights_node1 = {1.0, 1.0, 1.0, 1.0}; // Node 1 weights
    std::vector<double> weights_node2 = {1.0, 1.0, 1.0, 1.0}; // Node 2 weights

    // Create nodes with specified activation functions
    Node node1(weights_node1, Node::ActivationType::Linear);
    Node node2(weights_node2, Node::ActivationType::Tanh);

    // Create a layer with the nodes
    std::vector<Node> nodes = {node1, node2};
    Layer layer(nodes);

    // Define input values
    std::vector<double> inputs = {1.0, 1.0, 1.0};

    // Perform a forward pass
    std::vector<double> outputs = layer.forward(inputs);

    // Print the outputs
    std::cout << "Layer outputs: ";
    for (double output : outputs) {
        std::cout << output << " ";
    }
    std::cout << std::endl;

    return 0;
}
