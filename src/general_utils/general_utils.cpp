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

// Get index of highest value in a vector
int highestValueInd(std::vector<double> vec) {
  if (vec.empty()) {
    return -1;
  }
  std::vector<double>::iterator it = std::max_element(vec.begin(), vec.end());
  return std::distance(vec.begin(), it);
}

// Turn node reports into a csv file for easy processing
bool processNodeReports(const std::string& shoreside_log_dir, const std::string& out_dir) {
  std::ifstream infile(shoreside_log_dir);
  std::string line;
  std::ofstream outfile(out_dir);

  // Stop if we can't open the output file
  if (!outfile.is_open()) {
    return false;
  }

  // Write the header
  outfile << "abe_x,abe_y\n";

  // Write out line-by-line from the node reports
  std::regex pattern(R"(NAME=([^,]+),X=([^,]+),Y=([^,]+))");
  while (std::getline(infile, line)) {
    // Ignore empty lines
    // Ignore lines that start with % sign
    if (line.empty()) continue;
    if (line[0] == '%') continue;

    std::smatch match;
    if (std::regex_search(line, match, pattern)) {
      // Not using name yet, but it will be important later
      // std::string name = match[1];
      std::string x_str = match[2];
      std::string y_str = match[3];
      outfile << x_str << "," << y_str << "\n";
    }
  }
  return true;
}

// Check if two csv files have exactly the same contents
bool csvFilesAreEqual(const std::string& file1, const std::string& file2, int verbose) {
  std::ifstream f1(file1), f2(file2);
  if (!f1.is_open() || !f2.is_open())
    return false;

  std::string line1, line2;
  while (true) {
    bool read1 = static_cast<bool>(std::getline(f1, line1));
    bool read2 = static_cast<bool>(std::getline(f2, line2));
    if (!read1 && !read2) {
      // both files ended
      if (verbose > 0) std::cout << "Csv files "<<file1<<" and "<<file2<<" are equal." << std::endl;
      return true;
    }
    if (read1 != read2) {
      // one file ended before the other
      if (verbose > 0) std::cout << "Between csv files "<<file1<<" and "<<file2<<", one file ended before the other." << std::endl;
      return false;
    }
    if (line1 != line2) {
      // lines differ
      if (verbose > 0) std::cout << "Different lines found between files "<<file1<<" and "<<file2<<". Lines "<<line1<<" and "<<line2<<" differ, respectively." << std::endl;
      return false;
    }
  }
}

// Take in a csv and output a csv with no duplicate rows
bool csvFilterDuplicateRows(const std::string& in_csv, const std::string& out_csv, int verbose) {
    std::ifstream infile(in_csv);
    if (!infile.is_open()) {
        if (verbose > 0) std::cout << "Failed to open input CSV: " << in_csv << std::endl;
        return false;
    }

    std::ofstream outfile(out_csv);
    if (!outfile.is_open()) {
        if (verbose > 0) std::cout << "Failed to open output CSV: " << out_csv << std::endl;
        return false;
    }

    std::unordered_set<std::string> seen_rows;
    std::string line;

    while (std::getline(infile, line)) {
        if (seen_rows.insert(line).second) {  // Only insert if not already seen
            outfile << line << '\n';
        }
    }

    return true;
}
