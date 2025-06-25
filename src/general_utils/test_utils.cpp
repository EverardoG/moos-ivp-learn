#include "general_utils.h"
#include <iostream>
#include <vector>


bool test_calcDeltaHeading(int test_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- test_calcDeltaHeading()" << std::endl;
    // Simple test
    double delta_heading = calcDeltaHeading(270.0, 90.0);
    if (test_verbose > 0) std::cout << "first delta_heading: " << delta_heading << std::endl;
    if (!isClose(delta_heading, -180.0)) return false;
    // Now test the other way around
    delta_heading = calcDeltaHeading(90.0, 270.0);
    if (test_verbose > 0) std::cout << "second delta_heading: " << delta_heading << std::endl;
    if (!isClose(delta_heading, 180.0)) return false;
    // Tiny delta accross 0
    delta_heading = calcDeltaHeading(10.0, 360.0);
    if (test_verbose > 0) std::cout << "third delta heading: " << delta_heading << std::endl;
    if (!isClose(delta_heading, -10.0)) return false;
    if (test_verbose > 0) std::cout << "Finish --- test_calcDeltaHeading()" << std::endl;
    return true;
}

bool test_reshapeVector2D(int test_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- test_reshapeVector2D()" << std::endl;
    std::vector<double> flat_vector = {-1.0, 1.0, -180.0, 180.0};
    std::vector<std::vector<double>> reshaped_vec = reshapeVector2D(flat_vector, 2, 2);

    // Check shape
    if (reshaped_vec.size() != 2) return false;
    if (reshaped_vec[0].size() != 2) return false;
    if (reshaped_vec[1].size() != 2) return false;

    // Check values
    if (reshaped_vec[0][0] != -1.0) return false;
    if (reshaped_vec[0][1] != 1.0) return false;
    if (reshaped_vec[1][0] != -180.0) return false;
    if (reshaped_vec[1][1] != 180.0) return false;

    if (test_verbose > 0) std::cout << "Finish --- test_reshapeVector2D()" << std::endl;
    return true;
}

bool test_sectorToAngle(int test_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- test_sectorToAngle()" << std::endl;
    double angle = sectorToAngle(4, 0);
    if (test_verbose > 0) std::cout << "First angle (expected 0.0): " << angle << std::endl;
    if (!isClose(angle, 0.0)) return false;
    angle = sectorToAngle(4,1);
    if (test_verbose > 0) std::cout << "Second angle (expected 90.0): " << angle << std::endl;
    if (!isClose(angle, 90.0)) return false;
    angle = sectorToAngle(4,2);
    if (test_verbose > 0) std::cout << "Third angle (expected 180.0): " << angle << std::endl;
    if (!isClose(angle, 180.0)) return false;
    angle = sectorToAngle(4,3);
    if (test_verbose > 0) std::cout << "Fourth angle (expected 270.0): " << angle << std::endl;
    if (!isClose(angle, 270.0)) return false;
    if (test_verbose > 0) std::cout << "Finish --- test_sectorToAngle()" << std::endl;
    return true;
}

bool test_readingToXY(int test_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- test_readingToXY()" << std::endl;
    int num_sectors = 4;

    // Sector 0
    XYPoint point = readingToXY(num_sectors, 0, 1.0);
    if (test_verbose > 0) std::cout << "Sector 0 Point: (" << point.get_vx() << "," << point.get_vy() << ") | Expected: (0,1)" << std::endl;
    if (!isClose(point.get_vx(), 0.0)) return false;
    if (!isClose(point.get_vy(), 1.0)) return false;

    // Sector 1
    point = readingToXY(num_sectors, 1, 1.0);
    if (test_verbose > 0) std::cout << "Sector 1 Point: (" << point.get_vx() << "," << point.get_vy() << ") | Expected: (1,0)" << std::endl;
    if (!isClose(point.get_vx(), 1.0, 1.0E-9, 1.0E-14)) return false;
    if (!isClose(point.get_vy(), 0.0, 1.0E-9, 1.0E-14)) return false;

    // Sector 2
    point = readingToXY(num_sectors, 2, 1.0);
    if (test_verbose > 0) std::cout << "Sector 2 Point: (" << point.get_vx() << "," << point.get_vy() << ") | Expected: (0,-1)" << std::endl;
    if (!isClose(point.get_vx(), 0.0, 1.0E-9, 1.0E-14)) return false;
    if (!isClose(point.get_vy(), -1.0, 1.0E-9, 1.0E-14)) return false;

    // Sector 3
    point = readingToXY(num_sectors, 3, 1.0);
    if (test_verbose > 0) std::cout << "Sector 3 Point: (" << point.get_vx() << "," << point.get_vy() << ") | Expected: (-1,0)" << std::endl;
    if (!isClose(point.get_vx(), -1.0, 1.0E-9, 1.0E-14)) return false;
    if (!isClose(point.get_vy(), 0.0, 1.0E-9, 1.0E-14)) return false;

    if (test_verbose > 0) std::cout << "Finish --- test_readingToXY()" << std::endl;
    return true;
}

bool test_sumXY(int test_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- test_sumXY()" << std::endl;
    std::vector<XYPoint> points = {
        XYPoint(0.0, 2.0), XYPoint(-3.0, -3.0), XYPoint(11.0, -1.0)
    };
    XYPoint sum_pt = sumXY(points);
    if (test_verbose > 0) std::cout << "Sum XY Point: (" << sum_pt.get_vx() << "," << sum_pt.get_vy() << ") Expected: (8,-2)" << std::endl;
    if (!isClose(sum_pt.get_vx(), 8.0)) return false;
    if (!isClose(sum_pt.get_vy(), -2.0)) return false;
    if (test_verbose > 0) std::cout << "Finish --- test_sumXY()" << std::endl;
    return true;
}

bool test_averageXY(int test_verbose = 0) {
    if (test_verbose > 0) std::cout << "Start --- test_averageXY()" << std::endl;
    std::vector<XYPoint> points = {
        XYPoint(0.0, 2.0), XYPoint(-3.0, -3.0), XYPoint(11.0, -1.0)
    };
    XYPoint avg_pt = averageXY(points);
    if (test_verbose > 0) std::cout << "Average XY Point: (" << avg_pt.get_vx() << "," << avg_pt.get_vy() << ") Expected: (8,-2)" << std::endl;
    if (!isClose(avg_pt.get_vx(), 8.0/3.0)) return false;
    if (!isClose(avg_pt.get_vy(), -2.0/3.0)) return false;
    if (test_verbose > 0) std::cout << "Finish --- test_averageXY()" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    int TEST_VERBOSE = 0;
    if (argc >= 2) {
        // Conver the first argument to int (argv[1] is a string)
        TEST_VERBOSE = std::stoi(argv[1]);
    }

    // Test delta heading
    if (!test_calcDeltaHeading(TEST_VERBOSE)) std::cout << "FAILURE: test_calcDeltaHeading" << std::endl;
    else std::cout << "PASSED: test_calcDeltaHeading" << std::endl;

    // Test reshaping vector
    if (!test_reshapeVector2D(TEST_VERBOSE)) std::cout << "FAILURE: test_reshapeVector2D" << std::endl;
    else std::cout << "PASSED: test_reshapeVector2D" << std::endl;

    // Test getting angle for a sector
    if (!test_sectorToAngle(TEST_VERBOSE)) std::cout << "FAILURE: test_sectorToAngle" << std::endl;
    else std::cout << "PASSED: test_sectorToAngle" << std::endl;

    // Test getting XY from a sector reading
    if (!test_readingToXY(TEST_VERBOSE)) std::cout << "FAILURE: test_readingToXY" << std::endl;
    else std::cout << "PASSED: test_readingToXY" << std::endl;

    // Test summing XY points
    if (!test_sumXY(TEST_VERBOSE)) std::cout << "FAILURE: test_sumXY" << std::endl;
    else std::cout << "PASSED: test_sumXY" << std::endl;

    // Test averaging XY points
    if (!test_averageXY(TEST_VERBOSE)) std::cout << "FAILURE: test_averageXY" << std::endl;
    else std::cout << "PASSED: test_averageXY" << std::endl;
}
