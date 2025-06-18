#include "sector_sensor.h"

using Bucket = std::vector<double>;
using Buckets = std::vector<Bucket>;
using Entities = std::vector<XYPoint>;

// Transform list of XY points into sensor readings
std::vector<double> SectorSensor::query(Entities entities, double self_x, double self_y, double self_heading) {
    Buckets buckets = fillBuckets(entities, self_x, self_y, self_heading);
    return bucketsToReadings(buckets);
}

// Create buckets for sensing
Buckets SectorSensor::fillBuckets(Entities entities, double self_x, double self_y, double self_heading) {
    Buckets buckets(m_number_sectors);
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
    if (bucket_ind == m_number_sectors) bucket_ind = 0;
    buckets[bucket_ind].push_back(dist);
    }

    return buckets;
}

// Turn buckets into sensor readings
std::vector<double> SectorSensor::bucketsToReadings(Buckets buckets) {
    // Create our reading - vector of doubles
    std::vector<double> readings;

    // Go through each bucket. Populate readings one at a time. One per bucket
    for (const Bucket& bucket : buckets) {
        readings.push_back(composeReading(bucket));
    }

    // Normalize readings according to normalization rule
    if (m_normalization_rule == NormalizationRule::FIXED){
        if (m_verbosity_level > 1) std::cout << "norm rule is fixed" << std::endl;
        if (m_verbosity_level > 1) std::cout << "size of readings: " << readings.size() << std::endl;
        for (int i = 0; i<readings.size(); i++) {
            readings[i] = readings[i] / m_fixed_normalization_factor;
            if (m_verbosity_level > 1) std::cout << "readings["<<i<<"]"<<readings[i] << std::endl;
        }
    }
    else if (m_normalization_rule == NormalizationRule::DYNAMIC) {
        // Determine dynamic normalization factor
        double dynamic_norm_factor = 0;
        for (const Bucket& bucket : buckets) {
            dynamic_norm_factor = dynamic_norm_factor + bucket.size();
        }
        // Apply dynamic normalization factor (only if the norm factor is more than zero)
        if (dynamic_norm_factor > 0) {
            for (int i = 0; i<readings.size(); i++) {
                readings[i] = readings[i] / dynamic_norm_factor;
            }
        }
    }
    else if (m_normalization_rule != NormalizationRule::NONE) {
        if (m_verbosity_level > 0) std::cerr << "SectorSensor Warning: Invalid normalization rule specified. Not applying any normalization to sensor readings." << std::endl;
    }

    // Return the readings
    return readings;
};

// std::vector<double> SectorSensor::normalizeReadings(std::vector<double> readings) {

// }

// Helper function. Turn measurements in a bucket into a reading
double SectorSensor::composeReading(Bucket bucket) {
    double reading = 0;
    for (const double& dist : bucket) {
        if (dist > m_sensor_rad) {
            if (m_verbosity_level > 0) std::cerr << "SectorSensor Warning: Entity distance is out of range, but still being processed in composeReading(). Setting reading for this entity as 0." << std::endl;
            reading += 0;
        }
        else{
            reading += - (dist - m_saturation_rad) / (m_sensor_rad - m_saturation_rad) + 1.0;
        }
    }
    return reading;
}

SectorSensor::~SectorSensor() {
    // No custom cleanup needed
}
