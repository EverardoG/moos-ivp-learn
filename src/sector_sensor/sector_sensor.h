#include "XYFormatUtilsPoint.h"
#include "XYPoint.h"
#include "AngleUtils.h"      // for relAngle
#include <cmath>
// #include "general_utils.h"

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
    std::vector<double> query(Entities entities, double self_x, double self_y, double self_heading) {
      Buckets buckets = fillBuckets(entities, self_x, self_y, self_heading);
      return bucketsToReadings(buckets);
    }

    // Create buckets for sensing
    Buckets fillBuckets(Entities entities, double self_x, double self_y, double self_heading) {
      Buckets buckets;
      for (int i=0; i<entities.size(); i++){

        // How far away is this swimmer?
        double dx = self_x - entities[i].get_vx();
        double dy = self_y - entities[i].get_vy();
        double dist = sqrt(dx*dx + dy*dy);

        // Skip this swimmer if it's too far away
        if (dist > m_sensor_rad)
          continue;

        // What is the relative heading to this swimmer?
        double swimmer_heading = relAng(self_x, self_y,
            entities[i].get_vx(), entities[i].get_vy());


        // convert to local angle from straight ahead
        // (This angle represents the difference between the current heading and the heading required to go to the siwmmer)
        double angle_delta = calcDeltaHeading(self_heading, swimmer_heading);

        // Represent heading from 0 to 360
        angle_delta = angle360(angle_delta);

        // Then we need to take this angle, and represent it in the bucket-frame
        // Add sector_width/2.0 to the angle
        // Represent heading from 0 to 360
        // Now this angle represents which bucket this swimmer goes in
        double bucket_angle = angle_delta + m_sector_width/2.0;

        // Now divide the bucket_angle to get the bucket that this goes into
        // fill up the bucket
        int bucket_ind = bucket_angle / m_sector_width;
        buckets[bucket_ind].push_back(dist);
      }

      return buckets;
    }

    // Turn buckets into sensor readings
    std::vector<double> bucketsToReadings(Buckets buckets) {};

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

  private: // Configuration variables
    double m_sensor_rad;
    int    m_number_sectors;
    double m_sector_width;

  private: // State variables
    double x;
    double y;
    double heading;

  std::vector<double>  m_sensor_buckets;

  // std::vector<XYPoint> entities;
  // std::vector<bool>    entities_rescued;
};
