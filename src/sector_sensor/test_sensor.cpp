#include "sector_sensor.h"
#include <iostream>
#include <vector>

int main() {
    // Create a sector sensor for testing
    SectorSensor sensor = SectorSensor(10.0, 8);

    // 1) test that we can compose readings correctly
    // Create a bucket with distances inside
    // Make sure the reading is correct
    Bucket bucket_A = {5.0, 9.0, 3.3};
    double reading = sensor.composeReading(bucket_A);

    std::cout << "Reading from bucket A: " << reading << std::endl;

}
