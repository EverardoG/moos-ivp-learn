#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include "MBUtils.h"
#include <cmath>
#include <sstream>

//-------------------------------------------------------------
// Procedure: calcDeltaHeading(double heading1, double heading2)
//            the delta in degrees from heading1 to heading2
//            returns the delta heading in degrees [-180, 180]
double calcDeltaHeading(double heading1, double heading2);

//-------------------------------------------------------------
// Procedure: vectorToStream(const std::vector<double>& vec)
//            turn a vector of doubles into a stream for
//            easy printing to terminal
std::string vectorToStream(const std::vector<double>& vec);

// Function to check if two floating-point numbers are close
bool isClose(double a, double b, double rel_tol = 1e-9, double abs_tol = 0.0);

#endif // GENERAL_UTILS_H
