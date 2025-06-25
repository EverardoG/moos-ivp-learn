#include "general_utils.h"

//-------------------------------------------------------------
// Procedure: calcDeltaHeading(double heading1, double heading2)
//            the delta in degrees from heading1 to heading2
//            returns the delta heading in degrees [-180, 180]
double calcDeltaHeading(double heading1, double heading2)
{
  double deg_to_rad = M_PI/180.0;
  double rad_to_deg = 180.0/M_PI;

  // Unit vectors
  double a1 = sin(deg_to_rad*heading1);
  double a2 = cos(deg_to_rad*heading1);

  double b1 = sin(deg_to_rad*heading2);
  double b2 = cos(deg_to_rad*heading2);

  double k = a1*b2 - a2*b1;           // a cross b
  double dot_a_b = a1*b1+ a2*b2;      // a dot b

  // Clip for safety
  dot_a_b = vclip( dot_a_b, -1.0, 1.0);

  double delta_theta;
  if (k >=0){
    delta_theta = rad_to_deg * acos(dot_a_b) * (-1.0);
  } else {
    delta_theta = rad_to_deg * acos(dot_a_b) * (1.0);
  }

  return(delta_theta);
}

std::string vectorToStream(const std::vector<double>& vec, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i != vec.size() - 1) {
            oss << delimiter;
        }
    }
    return oss.str();
}

// Function to check if two floating-point numbers are close
bool isClose(double a, double b, double rel_tol, double abs_tol) {
    return std::abs(a - b) <= std::max(rel_tol * std::max(std::abs(a), std::abs(b)), abs_tol);
}

// Reshape a flat vector into a 2D vector with given rows and cols
std::vector<std::vector<double>> reshapeVector2D(
    const std::vector<double>& flat, size_t rows, size_t cols)
{
    std::vector<std::vector<double>> result;
    if (flat.size() != rows * cols) {
        // Handle error: size mismatch
        std::cerr << "reshapeVector2D(): Mismatched size vector." << std::endl;
        return result;
    }
    for (size_t i = 0; i < rows; ++i) {
        std::vector<double> row(flat.begin() + i * cols, flat.begin() + (i + 1) * cols);
        result.push_back(row);
    }
    return result;
}

bool setVecDoubleOnString(std::vector<double>& given_vec_double, const std::string& str, std::string& warning) {
  double temp_dbl;
  bool good_reading;

  // Turn our string into a vector of strings, using the comma as a delimiter
  std::vector<std::string> vec_str = parseString(str, ',');

  for (int i=0; i<vec_str.size(); i++){
    // Turn string into a double
    good_reading = setDoubleOnString(temp_dbl, vec_str[i]);

    if(!good_reading) {
      warning = "Bad reading in index: i=" + intToString(i)
		   + ", val=" + vec_str[i];
      return(false);
    }

    given_vec_double.push_back(temp_dbl);
  }
  return true;
}

bool setVecIntOnString(std::vector<int>& given_vec_int, const std::string& str, std::string& warning) {
  int temp_int;
  bool good_reading;

  // Turn our string into a vector of strings, using the comma as a delimiter
  std::vector<std::string> vec_str = parseString(str, ',');

  for (int i=0; i<vec_str.size(); i++){
    // Turn string into an int
    good_reading = setIntOnString(temp_int, vec_str[i]);

    if(!good_reading) {
      warning = "Bad reading in index: i=" + intToString(i)
		   + ", val=" + vec_str[i];
      return(false);
    }

    given_vec_int.push_back(temp_int);
  }
  return true;
}

//-------------------------------------------------------------
// Procedure: sectorToAngle
//   Purpose: Returns relative angle to the center of a sector
//   Example with 4 sectors:
//     Angles:                    |  Sectors:
//                   0            |             \     0     /
//                   |            |               \       /
//                   |            |                 \   /
//         270 ----- A ----- 90   |         3         A          1
//                   |            |                 /   \
//                   |            |               /       \
//                  180           |             /     2     \

double sectorToAngle(int num_sectors, int sector_id) {
  return 360.0*sector_id / (double)num_sectors;
}

XYPoint readingToXY(int num_sectors, int sector_id, double density) {
  double angle = sectorToAngle(num_sectors, sector_id);
  double x = density * sin(angle * M_PI/180.0);
  double y = density * cos(angle * M_PI/180.0);
  return XYPoint(x, y);
}

XYPoint sumXY(std::vector<XYPoint> pts) {
  double sum_x = 0.0;
  double sum_y = 0.0;
  for (const XYPoint& pt : pts) {
    sum_x = sum_x + pt.get_vx();
    sum_y = sum_y + pt.get_vy();
  }
  return XYPoint(sum_x, sum_y);
}

XYPoint averageXY(std::vector<XYPoint> pts) {
  double avg_x = 0.0;
  double avg_y = 0.0;
  for (const XYPoint& pt : pts) {
    avg_x = avg_x + pt.get_vx();
    avg_y = avg_y + pt.get_vy();
  }
  double num_pts_dbl = pts.size();
  avg_x = avg_x / num_pts_dbl;
  avg_y = avg_y / num_pts_dbl;
  return XYPoint(avg_x, avg_y);
}

// Angle relative to vehicle
//-------------------------------------------------------------
// Procedure: XYToRelAngle
//   Purpose: Returns relative angle to XYPoint
//     following this convention.
//     X is still horizontal and Y is still vertical
//   Example: (X=1, Y=0) gives angle 90
//                   0
//                   |
//                   |
//         270 ----- A ----- 90
//                   |
//                   |
//                  180

double XYToRelAngle(XYPoint point) {
  return relAng(XYPoint(0.0,0.0), point);
}
