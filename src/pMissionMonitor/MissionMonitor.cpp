/************************************************************/
/*    NAME: Everardo Gonzalez                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MissionMonitor.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include "MissionMonitor.h"

#include <iterator>

#include "ACTable.h"
#include "MBUtils.h"
#include "XYPolygon.h"  // Add this include for polygon support

using namespace std;

//---------------------------------------------------------
// Constructor()

MissionMonitor::MissionMonitor() {}

//---------------------------------------------------------
// Destructor

MissionMonitor::~MissionMonitor() {}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool MissionMonitor::OnNewMail(MOOSMSG_LIST &NewMail) {
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for (p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();

#if 0  // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if (key == "NODE_REPORT") {
      updateAgents(msg);
    }

    else if (key == "RESCUE_REGION") {
      updateMissionArea(msg);
    }

    else if (key != "APPCAST_REQ")  // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool MissionMonitor::OnConnectToServer() {
  registerVariables();
  return (true);
}

void MissionMonitor::updateMissionArea(CMOOSMsg &msg) {
  // Parse RESCUE_REGION which contains the geometry of the mission area
  string rescue_region = msg.GetString();
  // m_debug_messages += "Received RESCUE_REGION: " + rescue_region + "\n";

  // RESCUE_REGION format:
  // "pts={60,10:-75.5,-54.3:-37,-135.6:98.6,-71.3},edge_color=gray90,vertex_color=dodger_blue,vertex_size=5"

  // Find the pts field by looking for "pts={" and the matching "}"
  string pts_str = "";
  size_t pts_start = rescue_region.find("pts={");

  if (pts_start != string::npos) {
    // Move past "pts={"
    pts_start += 5;

    // Find the matching closing brace
    size_t pts_end = rescue_region.find("}", pts_start);
    if (pts_end != string::npos) {
      // Extract just the coordinates part (without the braces)
      pts_str = rescue_region.substr(pts_start, pts_end - pts_start);
      m_debug_messages += "Extracted pts_str: '" + pts_str + "'\n";
    }
  }

  if (!pts_str.empty()) {
    // Clear previous mission area
    m_mission_area.clear();

    // Parse points: "60,10:-75.5,-54.3:-37,-135.6:98.6,-71.3"
    // Points are separated by ':'
    vector<string> point_strs = parseString(pts_str, ':');

    for (const string &point_str : point_strs) {
      // Each point is "x,y"
      vector<string> coords = parseString(point_str, ',');

      if (coords.size() == 2) {
        try {
          double x = stod(coords[0]);
          double y = stod(coords[1]);
          m_mission_area.push_back(XYPoint(x, y));

          m_debug_messages += "Added mission area point: (" + to_string(x) +
                              "," + to_string(y) + ")\n";
        } catch (const std::exception &e) {
          m_debug_messages += "Error parsing point: " + point_str + "\n";
        }
      }
    }

    m_debug_messages += "Mission area updated with " +
                        to_string(m_mission_area.size()) + " points\n";
  }
}

bool MissionMonitor::isPointInMissionArea(const XYPoint &point) {
  if (m_mission_area.size() < 3) {
    return true;  // No valid polygon defined, consider all points valid
  }

  // Use ray casting algorithm to check if point is inside polygon
  double x = point.get_vx();
  double y = point.get_vy();
  bool inside = false;

  size_t n = m_mission_area.size();
  for (size_t i = 0, j = n - 1; i < n; j = i++) {
    double xi = m_mission_area[i].get_vx();
    double yi = m_mission_area[i].get_vy();
    double xj = m_mission_area[j].get_vx();
    double yj = m_mission_area[j].get_vy();

    if (((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi)) {
      inside = !inside;
    }
  }

  return inside;
}

void MissionMonitor::updateAgents(CMOOSMsg &msg) {
  // Parse NODE_REPORT which contains all vehicle info
  string node_report = msg.GetString();
  // m_debug_messages += "Received NODE_REPORT: " + node_report + "\n";

  // NODE_REPORT format:
  // "NAME=abe,TYPE=KAYAK,TIME=1234,X=100,Y=200,SPD=2.5,HDG=45,..." Extract
  // vehicle name, x, y from NODE_REPORT
  string vname = "";
  double x = 0, y = 0, hdg = 0;

  vector<string> parts = parseString(node_report, ',');
  for (const string &part : parts) {
    vector<string> keyval = parseString(part, '=');
    if (keyval.size() == 2) {
      if (keyval[0] == "NAME")
        vname = keyval[1];
      else if (keyval[0] == "X")
        x = stod(keyval[1]);
      else if (keyval[0] == "Y")
        y = stod(keyval[1]);
      else if (keyval[0] == "HDG")
        hdg = stod(keyval[1]);
    }
  }

  if (!vname.empty()) {
    // Update agent info
    XYPoint position(x, y, 0.0);
    bool in_area = isPointInMissionArea(position);

    if (m_agent_map.count(vname) > 0) {
      m_agent_map[vname].position.set_vx(x);
      m_agent_map[vname].position.set_vy(y);
      m_agent_map[vname].out_of_bounds = !in_area;

      m_debug_messages += "Updated agent " + vname + " at (" + to_string(x) +
                          "," + to_string(y) + ") - " +
                          (in_area ? "IN AREA" : "OUT OF BOUNDS") + "\n";
    } else {
      Agent new_agent(position);
      new_agent.out_of_bounds = !in_area;
      m_agent_map[vname] = new_agent;

      m_debug_messages += "Created new agent " + vname + " at (" +
                          to_string(x) + "," + to_string(y) + ") - " +
                          (in_area ? "IN AREA" : "OUT OF BOUNDS") + "\n";
    }
  }
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MissionMonitor::Iterate() {
  AppCastingMOOSApp::Iterate();

  // Check all agents for out of bounds status
  for (const std::pair<std::string, Agent> &entry : m_agent_map) {
    const std::string &agent_id = entry.first;
    const Agent &agent = entry.second;

    if (agent.out_of_bounds) {
      // Agent is out of bounds - could trigger alerts or actions here
      Notify("MISSION_EVALUATED", true);
      break;
    }
  }

  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MissionMonitor::OnStartUp() {
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if (!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for (p = sParams.begin(); p != sParams.end(); p++) {
    string orig = *p;
    string line = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if (param == "debug") {
      handled = setBooleanOnString(debug, value);
    }

    if (!handled) reportUnhandledConfigWarning(orig);
  }

  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void MissionMonitor::registerVariables() {
  AppCastingMOOSApp::RegisterVariables();
  // Register for NODE_REPORT which contains position info
  Register("NODE_REPORT", 0);
  // Register for RESCUE_REGION
  Register("RESCUE_REGION", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool MissionMonitor::buildReport() {
  m_msgs << "============================================" << endl;
  m_msgs << "File: MissionMonitor.cpp                       " << endl;
  m_msgs << "============================================" << endl;

  m_msgs << "Debug mode: " << (debug ? "ON" : "OFF") << std::endl;
  m_msgs << "Number of agents tracked: " << m_agent_map.size() << std::endl;
  m_msgs << "Mission area points: " << m_mission_area.size() << std::endl;
  m_msgs << "--------------------------------------------" << endl;

  // Show the data of agents being monitored
  if (m_agent_map.empty()) {
    m_msgs << "No agents currently tracked." << endl;
  } else {
    ACTable actab(5);
    actab << " Agent ID " << " X " << " Y " << " Status " << " Position ";
    actab.addHeaderLines();

    for (const std::pair<std::string, Agent> &entry : m_agent_map) {
      std::string id = entry.first;
      const Agent &agent = entry.second;
      string status = agent.out_of_bounds ? "OUT OF BOUNDS" : "IN AREA";
      actab << id << agent.position.get_vx() << agent.position.get_vy()
            << status << agent.position.get_spec();
    }

    m_msgs << actab.getFormattedString() << endl;
  }

  // Show mission area if available
  if (!m_mission_area.empty()) {
    m_msgs << "--------------------------------------------" << endl;
    m_msgs << "Mission Area Boundary:" << endl;

    ACTable area_table(3);
    area_table << " Point # " << " X " << " Y ";
    area_table.addHeaderLines();

    for (size_t i = 0; i < m_mission_area.size(); i++) {
      area_table << (int)(i + 1) << m_mission_area[i].get_vx()
                 << m_mission_area[i].get_vy();
    }

    m_msgs << area_table.getFormattedString() << endl;
  }

  // Show debug messages if debug is enabled and there are messages
  if (debug && !m_debug_messages.empty()) {
    m_msgs << "--------------------------------------------" << endl;
    m_msgs << "Debug Messages:" << endl;
    m_msgs << m_debug_messages;
    m_msgs << "--------------------------------------------" << endl;
    // Clear debug messages after displaying them
    m_debug_messages.clear();
  }

  return (true);
}
