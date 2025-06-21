#include "general_utils.h"
#include <iostream>
#include <vector>

int main() {
    double delta_heading = calcDeltaHeading(270.0, 90.0);
    std::cout << "Computed delta heading: " << delta_heading << std::endl;

    std::vector<double> flat_vector = {-1.0, 1.0, -180.0, 180.0};
    std::vector<std::vector<double>> reshaped_vec = reshapeVector2D(flat_vector, 2, 2);

}
