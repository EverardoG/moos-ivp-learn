#include "sector_sensor.h"
#include <iostream>
#include <sstream>
#include <vector>

std::string vectorToStream(const std::vector<double>& vec) {
    std::ostringstream oss;
    for (const double& value : vec) {
        oss << value << " ";
    }
    return oss.str();
}

void testComposeReadingOneEntity() {
    std::cout << "testComposeReadingOneEntity() --- Start" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );

    // Create a bucket with distances inside
    // Make sure the reading is correct
    Bucket bucket_A = {1.0};
    double reading_A = sensor.composeReading(bucket_A);
    std::cout << "Reading from bucket A: " << reading_A << std::endl;

    Bucket bucket_B = {2.0};
    double reading_B = sensor.composeReading(bucket_B);
    std::cout << "Reading from bucket B: " << reading_B << std::endl;

    Bucket bucket_C = {5.0};
    double reading_C = sensor.composeReading(bucket_C);
    std::cout << "Reading from bucket D: " << reading_C << std::endl;

    Bucket bucket_D = {10.0};
    double reading_D = sensor.composeReading(bucket_D);
    std::cout << "Reading from bucket D: " << reading_D << std::endl;

    Bucket bucket_E = {12.0};
    double reading_E = sensor.composeReading(bucket_E);
    std::cout << "Reading from bucket E: " << reading_E << std::endl;

    std::cout << "testComposeReadingOneEntity() --- Finish" << std::endl;
}

void testComposeReadingMultiEntity() {
    std::cout << "testComposeReadingMultiEntity() --- Start" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );

    // Create buckets with distances inside. Make sure readings are correct
    Bucket bucket_A = {1.0, 1.0, 1.0};
    double reading_A = sensor.composeReading(bucket_A);
    std::cout << "Reading from bucket A: " << reading_A << std::endl;

    Bucket bucket_B = {1.0, 2.0, 3.0};
    double reading_B = sensor.composeReading(bucket_B);
    // Expected: 2.66667
    std::cout << "Reading from bucket B: " << reading_B << std::endl;

    std::cout << "testComposeReadingMultiEntity() --- Finish" << std::endl;
}

void testBucketsToReadings() {
    std::cout << "testBucketsToReadings() --- Start" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );

    // Create buckets for turning into readings
    // Check that the reading is correct
    Buckets buckets_A = {
        {1.0, 5.0},
        {1.0, 1.0, 1.0, 1.0},
        {9.0, 9.0, 1.0},
        {1000.0},
        {}
    };
    std::vector<double> readings = sensor.bucketsToReadings(buckets_A);
    std::cout << "Readings from buckets_A: " << vectorToStream(readings) << std::endl;
    std::cout << "testBucketsToReadings() --- Finish" << std::endl;
}

int main() {
    // 1) Test that we can compose readings correctly
    testComposeReadingOneEntity();

    // 2) Test that we can compose readings correctly with multiple entities in a bucket
    testComposeReadingMultiEntity();

    // 3) Test that we can turn multiple buckets into sensor readings
    testBucketsToReadings();
}
