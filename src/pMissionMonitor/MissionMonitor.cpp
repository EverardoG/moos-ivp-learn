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

void MissionMonitor::updateAgents(CMOOSMsg &msg) {
  // Parse NODE_REPORT which contains all vehicle info
  string node_report = msg.GetString();
  m_debug_messages += "Received NODE_REPORT: " + node_report + "\n";

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
    if (m_agent_map.count(vname) > 0) {
      m_agent_map[vname].position.set_vx(x);
      m_agent_map[vname].position.set_vy(y);
      m_debug_messages += "Updated agent " + vname + " at (" + to_string(x) +
                          "," + to_string(y) + ")\n";
    } else {
      XYPoint position(x, y, 0.0);
      Agent new_agent(position);
      m_agent_map[vname] = new_agent;
      m_debug_messages += "Created new agent " + vname + " at (" +
                          to_string(x) + "," + to_string(y) + ")\n";
    }
  }
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MissionMonitor::Iterate() {
  AppCastingMOOSApp::Iterate();

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
}

//------------------------------------------------------------
// Procedure: buildReport()

bool MissionMonitor::buildReport() {
  m_msgs << "============================================" << endl;
  m_msgs << "File: MissionMonitor.cpp                       " << endl;
  m_msgs << "============================================" << endl;

  m_msgs << "Debug mode: " << (debug ? "ON" : "OFF") << std::endl;
  m_msgs << "Number of agents tracked: " << m_agent_map.size() << std::endl;
  m_msgs << "--------------------------------------------" << endl;

  // Show debug messages if debug is enabled and there are messages
  if (debug && !m_debug_messages.empty()) {
    m_msgs << "Debug Messages:" << endl;
    m_msgs << m_debug_messages;
    m_msgs << "--------------------------------------------" << endl;
    // Clear debug messages after displaying them
    m_debug_messages.clear();
  }

  if (m_agent_map.empty()) {
    m_msgs << "No agents currently tracked." << endl;
  } else {
    ACTable actab(4);
    actab << " Agent ID " << " X " << " Y " << " Position ";
    actab.addHeaderLines();

    for (const std::pair<std::string, Agent> &entry : m_agent_map) {
      std::string id = entry.first;
      const Agent &agent = entry.second;
      actab << id << agent.position.get_vx() << agent.position.get_vy()
            << agent.position.get_spec();
    }

    m_msgs << actab.getFormattedString();
  }

  return (true);
}
