#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include "MBUtils.h"
#include "XYPoint.h"
#include "AngleUtils.h"
#include <cmath>
#include <sstream>
#include <iostream>

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

double sectorToAngle(int num_sectors, int sector_id);

XYPoint readingToXY(int num_sectors, int sector_id, double density);

XYPoint sumXY(std::vector<XYPoint> pts);

XYPoint averageXY(std::vector<XYPoint> pts);

double XYToRelAngle(XYPoint point);

#endif // GENERAL_UTILS_H
