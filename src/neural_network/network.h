#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <cmath> // For std::tanh
#include <stdexcept> // For exceptions
#include <iostream> // For debug output

class Node {
public:
    enum class ActivationType { Tanh, Linear };

private:
    std::vector<double> weights;
    ActivationType activation_type;

    // Tanh activation function
    double tanh_activation(double x) {
        return std::tanh(x);
    }

    // Linear activation function
    double linear_activation(double x) {
        return x;
    }

public:
    // Constructor to initialize weights and activation type
    Node(const std::vector<double>& weights, ActivationType activation_type = ActivationType::Tanh)
        : weights(weights), activation_type(activation_type) {}

    // Forward method
    double forward(const std::vector<double>& inputs) {
        std::vector<double> extended_inputs = inputs;
        extended_inputs.push_back(1.0); // Append bias term

        double sum = 0.0;
        for (size_t i = 0; i < weights.size(); ++i) {
            sum += extended_inputs[i] * weights[i];
        }

        // Apply the specified activation function
        if (activation_type == ActivationType::Tanh) {
            return tanh_activation(sum);
        } else if (activation_type == ActivationType::Linear) {
            return linear_activation(sum);
        } else {
            throw std::invalid_argument("Unsupported activation type.");
        }
    }
};

class Layer {
private:
    std::vector<Node> nodes;

public:
    // Constructor to initialize the layer with pre-created nodes
    Layer(const std::vector<Node>& nodes) : nodes(nodes) {}

    // Forward method to compute outputs for all nodes in the layer
    std::vector<double> forward(const std::vector<double>& inputs) {
        std::vector<double> outputs;
        for (Node& node : nodes) {
            outputs.push_back(node.forward(inputs));
        }
        return outputs; // Return outputs from all nodes
    }
};

class NeuralNetwork {
private:
    std::vector<Layer> layers; // Each layer contains multiple nodes

public:
    // Constructor to initialize the network
    NeuralNetwork(const std::vector<double>& weights, const std::vector<int>& structure) {
        if (structure.size() < 3) {
            throw std::invalid_argument("Network structure must have at least an input, hidden, and output layer.");
        }

        size_t weight_index = 0;
        // For each layer
        for (size_t i = 0; i < structure.size() - 1; ++i) {
            int input_size = structure[i] + 1; // Include bias term
            int output_size = structure[i]; // Number of nodes in the current layer
            std::vector<Node> layer;

            std::cout << "Initializing layer " << i + 1 << " with " << output_size << " nodes." << std::endl;

            // For each node in this layer
            for (int j = 0; j < output_size; ++j) {
                if (weight_index + input_size > weights.size()) {
                    throw std::invalid_argument("Insufficient weights provided for the network structure.");
                }

                // Debug: Log weight allocation for each node
                std::cout << "Allocating weights for node " << j + 1 << " in layer " << i + 1 << ": ";
                for (size_t k = weight_index; k < weight_index + input_size; ++k) {
                    std::cout << weights[k] << " ";
                }
                std::cout << std::endl;

                // Correctly allocate weights for each node
                std::vector<double> node_weights(weights.begin() + weight_index, weights.begin() + weight_index + input_size);
                // Determine whether to use tanh or linear activation.
                Node node(node_weights, Node::ActivationType::Tanh);
                layer.emplace_back(node);
                weight_index += input_size;
            }
            layers.emplace_back(layer); // Add the layer to the network
        }

        if (weight_index != weights.size()) {
            std::cerr << "Error: Extra weights provided beyond the network structure." << std::endl;
            std::cerr << "Expected weights used: " << weight_index << ", but provided: " << weights.size() << std::endl;
            throw std::invalid_argument("Extra weights provided beyond the network structure.");
        }

        std::cout << "Neural network initialized successfully." << std::endl;
    }

    // Forward pass method
    std::vector<double> forward(const std::vector<double>& inputs) {
        std::vector<double> current_inputs = inputs;

        for (Layer& layer : layers) {
            current_inputs = layer.forward(current_inputs); // Outputs of the current layer become inputs for the next layer
        }

        return current_inputs; // Final outputs
    }
};

#endif // NETWORK_H
