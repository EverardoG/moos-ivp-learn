/************************************************************/
/*    NAME: Everardo Gonzalez, Tyler Paine                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_FollowCOM.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef FollowCOM_HEADER
#define FollowCOM_HEADER

#include <string>
#include <cmath>
#include "IvPBehavior.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "AngleUtils.h"
#include "general_utils.h"

class BHV_FollowCOM : public IvPBehavior {
public:
  BHV_FollowCOM(IvPDomain);
  ~BHV_FollowCOM() {};

  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();
  bool         processSensorReadings();
  bool         updateHeading();
  IvPFunction* buildFunction();
  bool         processHeading();

protected: // Local Utility functions

protected: // Configuration parameters
  int m_swimmer_sectors;
  int m_vehicle_sectors;
  int m_expected_total_sectors;
  bool m_sense_vehicles;

protected: // State variables
  double m_best_delta_heading;
  double m_best_speed;
  double m_nav_heading;
  std::vector<double> m_sector_sensor_readings;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain)
  {return new BHV_FollowCOM(domain);}
}
#endif
