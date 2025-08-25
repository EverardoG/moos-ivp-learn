#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include "MBUtils.h"
#include "XYPoint.h"
#include "AngleUtils.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <unordered_set>

//-------------------------------------------------------------
// Procedure: calcDeltaHeading(double heading1, double heading2)
//            the delta in degrees from heading1 to heading2
//            returns the delta heading in degrees [-180, 180]
double calcDeltaHeading(double heading1, double heading2);

//-------------------------------------------------------------
// Procedure: vectorToStream(const std::vector<double>& vec)
//            turn a vector of doubles into a stream for
//            easy printing to terminal
std::string vectorToStream(const std::vector<double>& vec, const std::string& delimiter = " ");

// Function to check if two floating-point numbers are close
bool isClose(double a, double b, double rel_tol = 1e-9, double abs_tol = 0.0);

// Reshape flat vector into 2D vector
std::vector<std::vector<double>> reshapeVector2D(
    const std::vector<double>& flat, size_t rows, size_t cols);

// Turn a string of comma seperated decimal numbers into a vector of doubles
bool setVecDoubleOnString(std::vector<double>& given_vec_double, const std::string& str, std::string& warning);

// Turn a string of comma seperated integers into a vector of integers
bool setVecIntOnString(std::vector<int>& given_vec_int, const std::string& str, std::string& warning);

// Turn a sector index into a 0->360 angle
double sectorToAngle(int num_sectors, int sector_id);

// Turn a sector reading into an XY point relative to sensor
XYPoint readingToXY(int num_sectors, int sector_id, double density);

// Sum many XY points into a single point
XYPoint sumXY(std::vector<XYPoint> pts);

// Take the average XY point of many XY points
XYPoint averageXY(std::vector<XYPoint> pts);

// Turn an XY point into a relative angle from a vehicle
double XYToRelAngle(XYPoint point);

// Get the index of the highest value in the vector
int highestValueInd(std::vector<double> vec);

// Turn node reports into a csv file for easy processing
bool processNodeReports(const std::string& shoreside_log_dir, const std::string& out_dir);

// Check if two csv files have exactly the same contents
bool csvFilesAreEqual(const std::string& file1, const std::string& file2, int verbose = 0);

// Take in a csv and output a csv with no duplicate rows
bool csvFilterDuplicateRows(const std::string& in_csv, const std::string& out_csv, int verbose = 0);

// Merge all *_positions.csv files in a directory into team_positions.csv
bool csvMergeFiles(const std::string& directory);

#endif // GENERAL_UTILS_H
