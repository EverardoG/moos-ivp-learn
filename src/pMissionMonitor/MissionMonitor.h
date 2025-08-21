/************************************************************/
/*    NAME: Everardo Gonzalez                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MissionMonitor.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef MissionMonitor_HEADER
#define MissionMonitor_HEADER

#include <cmath>
#include <unordered_map>
#include <unordered_set>

#include "AngleUtils.h" // for relAngle
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYFormatUtilsPoint.h"
#include "XYPoint.h"
#include "XYPolygon.h"
#include "general_utils.h"

struct Agent
{
  XYPoint position;
  bool out_of_bounds;
  Agent()
      : position(0.0, 0.0, 0.0), out_of_bounds(false) {} // Default constructor
  Agent(const XYPoint &pos) : position(pos), out_of_bounds(false) {}
};

class MissionMonitor : public AppCastingMOOSApp
{
public:
  MissionMonitor();
  ~MissionMonitor();

protected: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

protected:
  void registerVariables();

private: // Configuration variables
  bool debug;
  std::string termination_condition;
  double evaluation_area_offset;
  void updateAgents(CMOOSMsg &msg);
  void updateMissionArea(CMOOSMsg &msg);

private: // State variables
  double m_nav_x = 0.0;
  double m_nav_y = 0.0;
  double m_nav_hdg = 0.0;
  std::string m_agent_id;
  std::string m_debug_messages; // Accumulate debug messages

  std::unordered_map<std::string, Agent> m_agent_map;
  XYPolygon m_original_poly;
  XYPolygon m_offset_poly;
};

#endif
