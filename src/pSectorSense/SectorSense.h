/************************************************************/
/*    NAME: Everardo Gonzalez                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: SectorSense.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef SectorSense_HEADER
#define SectorSense_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYFormatUtilsPoint.h"
#include "XYPoint.h"
#include "AngleUtils.h"      // for relAngle
#include <cmath>
#include "general_utils.h"
#include "sector_sensor.h"

class SectorSense : public AppCastingMOOSApp
{
 public:
   SectorSense();
   ~SectorSense();

 protected: // Standard MOOSApp functions to overload
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void updateSwimmers();

 protected: // Standard AppCastingMOOSApp function to overload
   bool buildReport();

   double calcDeltaHeading(double heading1, double heading2);

 protected:
   void registerVariables();

 private: // Configuration variables
   double m_sensor_rad;
   int    m_number_sectors;
   double m_sector_width;
   double m_saturation_rad;

 private: // State variables
   double m_nav_x;
   double m_nav_y;
   double m_nav_hdg;

   std::vector<double>  m_sensor_buckets;

   std::vector<XYPoint> m_swimmers;
   std::vector<bool>    m_swimmers_rescued;
   std::vector<XYPoint> m_swimmers_sense;

   std::string m_sensor_readings_str;
   SectorSensor m_swimmer_sensor;
};

#endif
