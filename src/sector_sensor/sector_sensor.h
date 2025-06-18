#include "XYFormatUtilsPoint.h"
#include "XYPoint.h"
#include "AngleUtils.h"      // for relAngle
#include <cmath>
#include "general_utils.h"

using Bucket = std::vector<double>;
using Buckets = std::vector<Bucket>;
using Entities = std::vector<XYPoint>;

enum class NormalizationRule {
  NONE,
  FIXED,
  DYNAMIC
};

class SectorSensor {
  public:
    SectorSensor(
      const double& sensor_rad,
      const double& saturation_rad,
      const int& number_sectors,
      NormalizationRule normalization_rule = NormalizationRule::NONE,
      const double& fixed_normalization_factor = 1.0
    )
      : m_sensor_rad(sensor_rad),
      m_saturation_rad(saturation_rad),
      m_number_sectors(number_sectors),
      m_normalization_rule(normalization_rule),
      m_fixed_normalization_factor(fixed_normalization_factor)
      {
        m_sector_width = 360.0 / m_number_sectors;
        m_verbosity_level = 1;
      }
    ~SectorSensor();

    // Transform list of XY points into sensor readings
    std::vector<double> query(Entities entities, double self_x, double self_y, double self_heading);

    // Create buckets for sensing
    Buckets fillBuckets(Entities entities, double self_x, double self_y, double self_heading);

    // Turn buckets into sensor readings
    std::vector<double> bucketsToReadings(Buckets buckets);

    double composeReading(Bucket bucket);
    void setVerbose(int verbosity_level) {m_verbosity_level = verbosity_level;}
    int getVerbose() {return m_verbosity_level;}

  private: // Configuration variables
    double m_sensor_rad;
    double m_saturation_rad;
    int    m_number_sectors;
    double m_sector_width;
    NormalizationRule m_normalization_rule;
    double m_fixed_normalization_factor;
    int m_verbosity_level;

  private: // State variables
    double x;
    double y;
    double heading;

  std::vector<double>  m_sensor_buckets;
};
