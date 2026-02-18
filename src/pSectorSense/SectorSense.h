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
#include "ContactLedger.h"
#include <unordered_set>
#include <unordered_map>

struct Swimmer {
  XYPoint position;
  bool rescued;
  Swimmer(bool res) : rescued(res) {}
  Swimmer(const XYPoint& pos) : position(pos), rescued(false) {}
  Swimmer(const XYPoint& pos, bool res) : position(pos), rescued(res) {}
  Swimmer() : rescued(false) {}
};

class SectorSense : public AppCastingMOOSApp
{
 public:
   SectorSense();
   ~SectorSense();

   XYPolygon buildSectorPolygon(double cx, double cy, double heading_deg,
                             double sector_start_deg, double sector_end_deg,
                             double radius, int arc_points);
  void processSwimmerAlert(CMOOSMsg& msg);
  void processFoundSwimmer(CMOOSMsg& msg);
  void processVehicleReport(CMOOSMsg& msg);
  std::vector<XYPolygon> generatePolygons(std::vector<double> sensor_readings);
  std::vector<XYPolygon> generateVehiclePolygons(std::vector<double> sensor_readings);

 protected: // Standard MOOSApp functions to overload
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool handleTeammates();
   
   void updateSwimmers();
   void updateVehicles();
   

 protected: // Standard AppCastingMOOSApp function to overload
   bool buildReport();

   double calcDeltaHeading(double heading1, double heading2);

 protected:
   void registerVariables();

 private: // Configuration variables
   double m_sensor_rad;
   int    m_num_swimmer_sectors;
   int    m_num_vehicle_sectors;
   int    m_arc_points;
   double m_swim_sector_width;
   double m_vehicle_sector_width;
   double m_saturation_rad;
   bool   m_visualize_swim_sectors;
   bool   m_visualize_vehicle_sectors;
   bool   m_sense_vehicles;

 private: // State variables
   double m_nav_x=0.0;
   double m_nav_y=0.0;
   double m_nav_hdg=0.0;

   std::vector<double>  m_sensor_buckets;

   std::vector<XYPoint> m_swimmers;
   std::vector<bool>    m_swimmers_rescued;
   std::vector<XYPoint> m_swimmers_sense;
   std::unordered_set<int> m_swimmers_recorded;
   std::unordered_map<int, Swimmer> m_swimmer_map;

   std::string m_sensor_readings_str;
   std::string m_swimmer_readings_str;
   SectorSensor m_swimmer_sensor;

   // Vehicle sensing components
   ContactLedger m_contact_ledger;
   std::vector<XYPoint> m_vehicles_sense;
   SectorSensor m_vehicle_sensor;
   std::string m_vehicle_readings_str;

   std::string m_node_report;

   std::set<std::string> m_teammates; 
};

#endif
