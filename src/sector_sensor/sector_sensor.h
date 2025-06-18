#include "XYFormatUtilsPoint.h"
#include "XYPoint.h"
#include "AngleUtils.h"      // for relAngle
#include <cmath>
#include "general_utils.h"

using Bucket = std::vector<double>;
using Buckets = std::vector<Bucket>;
using Entities = std::vector<XYPoint>;

class SectorSensor {
  public:
    SectorSensor(const double& sensor_rad, const int& number_sectors)
      : m_sensor_rad(sensor_rad), m_number_sectors(number_sectors) {
        m_sector_width = 360.0 / m_number_sectors;
      }
    ~SectorSensor();

    // Transform list of XY points into sensor readings
    std::vector<double> query(Entities entities, double self_x, double self_y, double self_heading);

    // Create buckets for sensing
    Buckets fillBuckets(Entities entities, double self_x, double self_y, double self_heading);

    // Turn buckets into sensor readings
    std::vector<double> bucketsToReadings(Buckets buckets);

    double composeReading(Bucket bucket);

  private: // Configuration variables
    double m_sensor_rad;
    int    m_number_sectors;
    double m_sector_width;

  private: // State variables
    double x;
    double y;
    double heading;

  std::vector<double>  m_sensor_buckets;
};
