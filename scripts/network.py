import torch
import torch.nn as nn
import torch.nn.functional as F

class NeuralNetwork(nn.Module):
    def __init__(self, weights, structure, bounds):
        """
        weights: flat list of weights (float)
        structure: list of ints, e.g. [input, hidden1, ..., output]
        bounds: list of [min, max] for each output
        """
        super().__init__()
        self.structure = structure
        self.bounds = bounds

        # Build layers
        self.layers = nn.ModuleList()
        self.activations = []
        idx = 0
        for i in range(len(structure) - 1):
            in_features = structure[i] + 1  # +1 for bias
            out_features = structure[i+1]
            # Extract weights for this layer
            layer_weights = []
            for _ in range(out_features):
                w = weights[idx:idx+in_features]
                layer_weights.append(w)
                idx += in_features
            # Create linear layer
            linear = nn.Linear(structure[i], out_features)
            # Set weights and bias
            weight_tensor = torch.tensor([w[:-1] for w in layer_weights], dtype=torch.float32)
            bias_tensor = torch.tensor([w[-1] for w in layer_weights], dtype=torch.float32)
            linear.weight = nn.Parameter(weight_tensor)
            linear.bias = nn.Parameter(bias_tensor)
            self.layers.append(linear)
            # Use Tanh for all but last layer, Linear for last
            if i < len(structure) - 2:
                self.activations.append(torch.tanh)
            else:
                self.activations.append(lambda x: x)  # Linear

        if idx != len(weights):
            raise ValueError(f"Extra weights provided: used {idx}, provided {len(weights)}")
        if len(bounds) != structure[-1]:
            raise ValueError("Bounds size must match number of outputs.")

    def forward(self, x):
        if not isinstance(x, torch.Tensor):
            x = torch.tensor(x, dtype=torch.float32)
        for i, (layer, activation) in enumerate(zip(self.layers, self.activations)):
            # Add bias input
            if x.dim() == 1:
                x = torch.cat([x, torch.ones(1)], dim=0)
            else:
                x = torch.cat([x, torch.ones(x.shape[0], 1)], dim=1)
            # Remove bias for layer input
            x_in = x[:-1] if x.dim() == 1 else x[:, :-1]
            x = layer(x_in)
            x = activation(x)
        # Apply bounds to outputs
        x_out = x.clone()
        for i, (lo, hi) in enumerate(self.bounds):
            if self.activations[-1] == torch.tanh:
                # Asymmetric scaling for tanh
                x_out[i] = x[i] * (hi if x[i] > 0 else -lo)
            else:
                # Clamp for linear
                x_out[i] = torch.clamp(x[i], lo, hi)
        return x_out
