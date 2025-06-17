#include "general_utils.h"
#include <iostream>
#include <vector>

int main() {
    double delta_heading = calcDeltaHeading(270.0, 90.0);
    std::cout << "Computed delta heading: " << delta_heading << std::endl;
}
