#include "sector_sensor.h"

using Bucket = std::vector<double>;
using Buckets = std::vector<Bucket>;
using Entities = std::vector<XYPoint>;

// Transform list of XY points into sensor readings
// std::vector<double> SectorSensor::query(Entities entities, double self_x, double self_y, double self_heading) {
//     Buckets buckets = fillBuckets(entities, self_x, self_y, self_heading);
//     return bucketsToReadings(buckets);
// }


// Create buckets for sensing
// Buckets SectorSensor::fillBuckets(Entities entities, double self_x, double self_y, double self_heading) {
//     Buckets buckets;
//     for (int i=0; i<entities.size(); i++){

//     // How far away is this swimmer?
//     double dx = self_x - entities[i].get_vx();
//     double dy = self_y - entities[i].get_vy();
//     double dist = sqrt(dx*dx + dy*dy);

//     // Skip this swimmer if it's too far away
//     if (dist > m_sensor_rad)
//         continue;

//     // What is the relative heading to this swimmer?
//     double swimmer_heading = relAng(self_x, self_y,
//         entities[i].get_vx(), entities[i].get_vy());


//     // convert to local angle from straight ahead
//     // (This angle represents the difference between the current heading and the heading required to go to the siwmmer)
//     double angle_delta = calcDeltaHeading(self_heading, swimmer_heading);

//     // Represent heading from 0 to 360
//     angle_delta = angle360(angle_delta);

//     // Then we need to take this angle, and represent it in the bucket-frame
//     // Add sector_width/2.0 to the angle
//     // Represent heading from 0 to 360
//     // Now this angle represents which bucket this swimmer goes in
//     double bucket_angle = angle_delta + m_sector_width/2.0;

//     // Now divide the bucket_angle to get the bucket that this goes into
//     // fill up the bucket
//     int bucket_ind = bucket_angle / m_sector_width;
//     buckets[bucket_ind].push_back(dist);
//     }

//     return buckets;
// }

// Turn buckets into sensor readings
// std::vector<double> SectorSensor::bucketsToReadings(Buckets buckets) {};
