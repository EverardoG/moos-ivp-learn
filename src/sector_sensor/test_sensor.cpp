#include "sector_sensor.h"
#include "general_utils.h"
#include <iostream>
#include <sstream>
#include <vector>

bool testComposeReadingOneEntity(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- testComposeReadingOneEntity()" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );
    sensor.setVerbose(sense_verbose);

    // Create a bucket with distances inside
    // Make sure the reading is correct
    Bucket bucket_A = {1.0};
    double reading_A = sensor.composeReading(bucket_A);
    if (test_verbose > 0) std::cout << "Reading from bucket A: " << reading_A << std::endl;
    if (!isClose(reading_A, 1.0)) return false;


    Bucket bucket_B = {2.0};
    double reading_B = sensor.composeReading(bucket_B);
    if (test_verbose > 0) std::cout << "Reading from bucket B: " << reading_B << std::endl;
    if (!isClose(reading_B, 0.888889, 1.0E-5)) return false;

    Bucket bucket_C = {5.0};
    double reading_C = sensor.composeReading(bucket_C);
    if (test_verbose > 0) std::cout << "Reading from bucket C: " << reading_C << std::endl;
    if (!isClose(reading_C, 0.555556, 1.0E-5)) return false;

    Bucket bucket_D = {10.0};
    double reading_D = sensor.composeReading(bucket_D);
    if (test_verbose > 0) std::cout << "Reading from bucket D: " << reading_D << std::endl;
    if (!isClose(reading_D, 0.0)) return false;

    Bucket bucket_E = {12.0};
    double reading_E = sensor.composeReading(bucket_E);
    if (test_verbose > 0) std::cout << "Reading from bucket E: " << reading_E << std::endl;
    if (!isClose(reading_E, 0.0)) return false;

    if (test_verbose > 0) std::cout << "Finish --- testComposeReadingOneEntity()" << std::endl;
    return true;
}

bool testComposeReadingMultiEntity(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- testComposeReadingMultiEntity()" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );
    sensor.setVerbose(sense_verbose);

    // Create buckets with distances inside. Make sure readings are correct
    Bucket bucket_A = {1.0, 1.0, 1.0};
    double reading_A = sensor.composeReading(bucket_A);
    if (test_verbose > 0) std::cout << "Reading from bucket A: " << reading_A << std::endl;
    if (!isClose(reading_A, 3)) return false;

    Bucket bucket_B = {1.0, 2.0, 3.0};
    double reading_B = sensor.composeReading(bucket_B);
    if (test_verbose > 0) std::cout << "Reading from bucket B: " << reading_B << std::endl;
    if (!isClose(reading_B, 2.66667, 1.0E-5)) return false;

    if (test_verbose > 0) std::cout << "Finish --- testComposeReadingMultiEntity()" << std::endl;
    return true;
}

bool testBucketsToReadings(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start -- testBucketsToReadings() " << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );
    sensor.setVerbose(sense_verbose);

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
    if (test_verbose > 0) std::cout << "Readings from buckets_A: " << vectorToStream(readings) << std::endl;
    if (readings.size() != buckets_A.size()) return false;
    if (!isClose(readings[0], 1.55556, 1.0E-5)) return false;
    if (!isClose(readings[1], 4)) return false;
    if (!isClose(readings[2], 1.22222, 1.0E-5)) return false;
    if (!isClose(readings[3], 0)) return false;
    if (!isClose(readings[4], 0)) return false;
    if (test_verbose > 0) std::cout << "Finish --- testBucketsToReadings()" << std::endl;
    return true;
}

bool testFillingBuckets(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start -- testFillingBuckets() " << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );
    sensor.setVerbose(sense_verbose);

    // Start with the entities we want to sense
    Entities entities = {
        XYPoint(0,1),
        XYPoint(0,1),
        XYPoint(1,0),
        XYPoint(0,-1)
    };
    if (test_verbose > 0) std::cout << "Created XY points" << std::endl;

    // Now pretend we are at (0,0) and make sure we sense the entities correctly
    double vehicle_x = 0;
    double vehicle_y = 0;
    double vehicle_heading = 0;
    Buckets buckets = sensor.fillBuckets(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Filled buckets" << std::endl;

    // Check sizes
    // 4 buckets for 4 sectors
    // bucket 0 has 2 entities
    // bucket 1 has 1
    // bucket 2 has 1
    // bucket 3 has 0
    if (buckets.size() != 4) return false;
    if (buckets[0].size() != 2) return false;
    if (buckets[1].size() != 1) return false;
    if (buckets[2].size() != 1) return false;
    if (buckets[3].size() != 0) return false;
    if (test_verbose > 0) std::cout << "Checked sizes" << std::endl;

    // Check values
    // bucket 0 has 1,1
    // bucket 1 has 1
    // bucket 2 has 1
    // bucket 3 is empty
    if (!isClose(buckets[0][0], 1)) return false;
    if (!isClose(buckets[0][1], 1)) return false;
    if (!isClose(buckets[1][0], 1)) return false;
    if (!isClose(buckets[2][0], 1)) return false;
    if (test_verbose > 0) std::cout << "Checked values" << std::endl;

    // Now turn us 90 degrees
    vehicle_x = 0;
    vehicle_y = 0;
    vehicle_heading = 90;
    buckets = sensor.fillBuckets(entities, vehicle_x, vehicle_y, vehicle_heading);

    // Check sizes
    // bucket 0 has 1 entities
    // bucket 1 has 1
    // bucket 2 has 0
    // bucket 3 has 2
    if (buckets[0].size() != 1) return false;
    if (buckets[1].size() != 1) return false;
    if (buckets[2].size() != 0) return false;
    if (buckets[3].size() != 2) return false;
    if (test_verbose > 0) std::cout << "Checked sizes" << std::endl;

    // Check values
    // bucket 0 has 1
    // bucket 1 has 1
    // bucket 2 is empty
    // bucket 3 has 1,1
    if (!isClose(buckets[0][0], 1)) return false;
    if (!isClose(buckets[1][0], 1)) return false;
    if (!isClose(buckets[3][0], 1)) return false;
    if (!isClose(buckets[3][1], 1)) return false;
    if (test_verbose > 0) std::cout << "Checked values" << std::endl;

    // Now let's move the vehicle
    vehicle_x = 2;
    vehicle_y = 0;
    vehicle_heading = 90;
    buckets = sensor.fillBuckets(entities, vehicle_x, vehicle_y, vehicle_heading);

    // Check sizes
    // Everything is in bucket 2
    if (buckets[0].size() != 0) return false;
    if (buckets[1].size() != 0) return false;
    if (buckets[2].size() != 4) return false;
    if (buckets[3].size() != 0) return false;

    // Check values
    // Bucket 2 has {2.23607 2.23607 1 2.23607}
    if (!isClose(buckets[2][0], 2.23607, 1.0E-5)) return false;
    if (!isClose(buckets[2][1], 2.23607, 1.0E-5)) return false;
    if (!isClose(buckets[2][2], 1, 1.0E-5)) return false;
    if (!isClose(buckets[2][3], 2.23607, 1.0E-5)) return false;
    if (test_verbose > 0) std::cout << "Bucket 2 contains: " << vectorToStream(buckets[2]) << std::endl;

    if (test_verbose > 0) std::cout << "Finish --- testFillingBuckets()" << std::endl;
    return true;
}

bool testQuery(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- testQuery()" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );
    sensor.setVerbose(sense_verbose);

    // Create some entities to sense
    Entities entities = {
        XYPoint(0,1),
        XYPoint(0,1),
        XYPoint(1,0),
        XYPoint(0,-1)
    };
    if (test_verbose > 0) std::cout << "Created XY points" << std::endl;

    // Query the sensor for the readings
    double vehicle_x = 0;
    double vehicle_y = 0;
    double vehicle_heading = 0;
    std::vector<double> readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Queried sensor" << std::endl;
    if (test_verbose > 0) std::cout << "Sensor readings: " << vectorToStream(readings) << std::endl;

    // Check size. Check values.
    if (readings.size() != 4) return false;
    if (readings[0] != 2) return false;
    if (readings[1] != 1) return false;
    if (readings[2] != 1) return false;
    if (readings[3] != 0) return false;

    if (test_verbose > 0) std::cout << "Finish --- testQuery()" << std::endl;
    return true;
}

bool testOnTopQuery(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- testOnTopQuery()" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );
    sensor.setVerbose(sense_verbose);

    // Create some entities to sense
    Entities entities = {
        XYPoint(0,1),
        XYPoint(0,1),
        XYPoint(1,0),
        XYPoint(0,-1),
        XYPoint(0,0)
    };
    if (test_verbose > 0) std::cout << "Created XY points" << std::endl;

    // Query the sensor for the readings
    double vehicle_x = 0;
    double vehicle_y = 0;
    double vehicle_heading = 0;
    std::vector<double> readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Queried sensor" << std::endl;
    if (test_verbose > 0) std::cout << "Sensor readings: " << vectorToStream(readings) << std::endl;

    // Check size. Check values.
    if (readings.size() != 4) return false;
    if (readings[0] != 3) return false;
    if (readings[1] != 1) return false;
    if (readings[2] != 1) return false;
    if (readings[3] != 0) return false;

    if (test_verbose > 0) std::cout << "Finish --- testOnTopQuery()" << std::endl;
    return true;
}


bool testFixedNorm(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- testFixedNorm()" << std::endl;
    // Create a sector sensor for testing with fixed norm of 10
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    NormalizationRule norm_rule = NormalizationRule::FIXED;
    double fixed_norm_factor = 10;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors,
        norm_rule,
        fixed_norm_factor
    );
    sensor.setVerbose(sense_verbose);

    // Create some entities to sense
    Entities entities = {
        XYPoint(0,1),
        XYPoint(0,1),
        XYPoint(1,0),
        XYPoint(0,-1)
    };
    if (test_verbose > 0) std::cout << "Created XY points" << std::endl;

    // Query the sensor
    double vehicle_x = 0;
    double vehicle_y = 0;
    double vehicle_heading = 0;
    std::vector<double> readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Queried sensor" << std::endl;
    if (test_verbose > 0) std::cout << "Sensor readings: " << vectorToStream(readings) << std::endl;

    // Check size. Check values.
    if (readings.size() != 4) return false;
    if (!isClose(readings[0], 0.2)) return false;
    if (!isClose(readings[1], 0.1)) return false;
    if (!isClose(readings[2], 0.1)) return false;
    if (!isClose(readings[3], 0)) return false;

    if (test_verbose > 0) std::cout << "Finish --- testFixedNorm()" << std::endl;
    return true;
}

bool testDynamicNorm(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- testDynamicNorm()" << std::endl;
    // Create a sector sensor for testing with dynamic norm
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 4;
    NormalizationRule norm_rule = NormalizationRule::DYNAMIC;
    double fixed_norm_factor = 10;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors,
        norm_rule,
        fixed_norm_factor
    );
    sensor.setVerbose(sense_verbose);

    // Create some entities to sense
    Entities entities = {
        XYPoint(0,1),
        XYPoint(0,1),
        XYPoint(1,0),
        XYPoint(0,-1)
    };
    if (test_verbose > 0) std::cout << "Created XY points" << std::endl;

    // Query the sensor
    double vehicle_x = 0;
    double vehicle_y = 0;
    double vehicle_heading = 0;
    std::vector<double> readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Queried sensor" << std::endl;
    if (test_verbose > 0) std::cout << "Sensor readings: " << vectorToStream(readings) << std::endl;

    // Check size. Check values.
    if (readings.size() != 4) return false;
    if (!isClose(readings[0], 0.5)) return false;
    if (!isClose(readings[1], 0.25)) return false;
    if (!isClose(readings[2], 0.25)) return false;
    if (!isClose(readings[3], 0)) return false;

    if (test_verbose > 0) std::cout << "First query was successful" << std::endl;

    // Check edge case where no entities are present
    entities = {};
    if (test_verbose > 0) std::cout << "Created empty entities vector" << std::endl;

    // Query the sensor. Don't move the vehicle.
    readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Queried sensor" << std::endl;
    if (test_verbose > 0) std::cout << "Sensor readings: " << vectorToStream(readings) << std::endl;

    // Check size. Check values.
    if (readings.size() != 4) return false;
    if (!isClose(readings[0], 0)) return false;
    if (!isClose(readings[1], 0)) return false;
    if (!isClose(readings[2], 0)) return false;
    if (!isClose(readings[3], 0)) return false;

    if (test_verbose > 0) std::cout << "Finish --- testDynamicNorm()" << std::endl;
    return true;
}

bool testNumSectors(int test_verbose = 0, int sense_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- testDynamicNorm()" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 8;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );
    sensor.setVerbose(sense_verbose);

    // Create some entities to sense
    Entities entities = {
        XYPoint(0,1),
        XYPoint(0,1),
        XYPoint(1,0),
        XYPoint(0,-1)
    };
    if (test_verbose > 0) std::cout << "Created XY points" << std::endl;

    // Query the sensor for the readings
    double vehicle_x = 0;
    double vehicle_y = 0;
    double vehicle_heading = 0;
    std::vector<double> readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Queried sensor" << std::endl;
    if (test_verbose > 0) std::cout << "Sensor readings: " << vectorToStream(readings) << std::endl;

    // Check size. Check values.
    if (readings.size() != 8) return false;
    if (readings[0] != 2) return false;
    if (readings[1] != 0) return false;
    if (readings[2] != 1) return false;
    if (readings[3] != 0) return false;
    if (readings[4] != 1) return false;
    if (readings[5] != 0) return false;
    if (readings[6] != 0) return false;
    if (readings[7] != 0) return false;

    // Let's do this again with 40 sectors, just to push the limits a bit
    num_sectors = 40;
    sensor = SectorSensor(sensor_rad, saturation_rad, num_sectors);
    readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);

    // Check size. Check values.
    // Most values will be zero.
    if (readings.size() != 40) return false;
    for (int i = 0; i<readings.size(); i++) {
        if (i == 0) {
            if (readings[i] != 2) return false;
        } else if (i == 10) {
            if (readings[i] != 1) return false;
        } else if (i == 20) {
            if (readings[i] != 1) return false;
        } else {
            if (readings[i] != 0) return false;
        }
    }

    if (test_verbose > 0) std::cout << "Finish --- testDynamicNorm()" << std::endl;
    return true;
}

bool testMultiQuery(int test_verbose = 0, int sense_verbose=0) {
    if (test_verbose > 0) std::cout << "Start --- testMultiQuery()" << std::endl;
    // Create a sector sensor for testing
    double sensor_rad = 10.0;
    double saturation_rad = 1.0;
    int num_sectors = 8;
    SectorSensor sensor = SectorSensor(
        sensor_rad,
        saturation_rad,
        num_sectors
    );
    sensor.setVerbose(sense_verbose);

    // Create some entities to sense
    Entities entities = {
        XYPoint(0,1),
        XYPoint(0,1),
        XYPoint(1,0),
        XYPoint(0,-1)
    };
    if (test_verbose > 0) std::cout << "Created XY points" << std::endl;

    // Query the sensor for the readings
    double vehicle_x = 0;
    double vehicle_y = 0;
    double vehicle_heading = 0;
    std::vector<double> readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Queried sensor" << std::endl;
    if (test_verbose > 0) std::cout << "Sensor readings: " << vectorToStream(readings) << std::endl;

    // Check size. Check values.
    if (readings.size() != 8) return false;
    if (readings[0] != 2) return false;
    if (readings[1] != 0) return false;
    if (readings[2] != 1) return false;
    if (readings[3] != 0) return false;
    if (readings[4] != 1) return false;
    if (readings[5] != 0) return false;
    if (readings[6] != 0) return false;
    if (readings[7] != 0) return false;

    // Query the sensor again. Same input. Output should be exactly the same.
    readings = sensor.query(entities, vehicle_x, vehicle_y, vehicle_heading);
    if (test_verbose > 0) std::cout << "Queried sensor (again)" << std::endl;
    if (test_verbose > 0) std::cout << "Sensor readings: " << vectorToStream(readings) << std::endl;

    // Check size. Check values.
    if (readings.size() != 8) return false;
    if (readings[0] != 2) return false;
    if (readings[1] != 0) return false;
    if (readings[2] != 1) return false;
    if (readings[3] != 0) return false;
    if (readings[4] != 1) return false;
    if (readings[5] != 0) return false;
    if (readings[6] != 0) return false;
    if (readings[7] != 0) return false;

    if (test_verbose > 0) std::cout << "Finish --- testMultiQuery()" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    int TEST_VERBOSE = 0;
    int SENSE_VERBOSE = 0;
    if (argc >= 2) {
        // Convert the first argument to int (argv[1] is a string)
        TEST_VERBOSE = std::stoi(argv[1]);
    }
    if (argc >= 3) {
        SENSE_VERBOSE = std::stoi(argv[2]);
    }

    // 1) Test that we can compose readings correctly
    if (!testComposeReadingOneEntity(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testComposeReadingOneEntity" << std::endl;
    else std::cout << "PASSED: testComposeReadingOneEntity" << std::endl;

    // 2) Test that we can compose readings correctly with multiple entities in a bucket
    if (!testComposeReadingMultiEntity(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testComposeReadingMultiEntity"<< std::endl;
    else std::cout << "PASSED: testComposeReadingMultiEntity" << std::endl;

    // 3) Test that we can turn multiple buckets into sensor readings
    if (!testBucketsToReadings(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testBucketsToReadings" << std::endl;
    else std::cout << "PASSED: testBucketsToReadings" << std::endl;

    // 4) Test that we can fill buckets correctly
    if (!testFillingBuckets(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testFillingBuckets" << std::endl;
    else std::cout << "PASSED: testFillingBuckets" << std::endl;

    // 5) Test that we can query all the way from entities to readings
    if (!testQuery(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testQuery" << std::endl;
    else std::cout << "PASSED: testQuery" << std::endl;

    // 5A) Make sure this works if we are on top of the entities
    if (!testOnTopQuery(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testOnTopQuery" << std::endl;
    else std::cout << "PASSED: testOnTopQuery" << std::endl;

    // 6) Test fixed normalization
    if (!testFixedNorm(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testFixedNorm" << std::endl;
    else std::cout << "PASSED: testFixedNorm" << std::endl;

    // 7) Test dynamic normalization
    if (!testDynamicNorm(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testDynamicNorm" << std::endl;
    else std::cout << "PASSED: testDynamicNorm" << std::endl;

    // 8) Test using different numbers of sectors
    if (!testNumSectors(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testNumSectors" << std::endl;
    else std::cout << "PASSED: testNumSectors" << std::endl;

    // 9) Test querying the sensor multiple times
    if (!testMultiQuery(TEST_VERBOSE, SENSE_VERBOSE)) std::cout << "FAILURE: testMultiQuery" << std::endl;
    else std::cout << "PASSED: testMultiQuery" << std::endl;

}
