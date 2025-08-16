/************************************************************/
/*    NAME: Everardo Gonzalez                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: SectorSense.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "SectorSense.h"
#include "XYPolygon.h" // Add this include for polygon support

using namespace std;

//---------------------------------------------------------
// Constructor()

SectorSense::SectorSense()
{
}

//---------------------------------------------------------
// Destructor

SectorSense::~SectorSense()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool SectorSense::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "SWIMMER_ALERT") {
      processSwimmerAlert(msg);
    } else if (key == "FOUND_SWIMMER") {
      processFoundSwimmer(msg);
    } else if (key == "NAV_X"){
      m_nav_x = msg.GetDouble();
    } else if (key == "NAV_Y"){
      m_nav_y = msg.GetDouble();
    } else if (key == "NAV_HEADING"){
      m_nav_hdg = msg.GetDouble();
    } else if (key == "NODE_REPORT") {
      if (m_sense_vehicles) {
        processVehicleReport(msg);
      }
      m_node_report = msg.GetString();
    }


    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool SectorSense::OnConnectToServer()
{
  //registerVariables();
   return(true);
}

void SectorSense::updateSwimmers() {
  m_swimmers_sense.clear();
  for (const std::pair<int, Swimmer>& entry : m_swimmer_map) {
    int id = entry.first;
    const Swimmer& s = entry.second;
    if (!s.rescued) {
      m_swimmers_sense.push_back(s.position);
    }
  }
}

void SectorSense::updateVehicles() {
  m_vehicles_sense.clear();
  if (!m_sense_vehicles) return;

  std::vector<std::string> vnames = m_contact_ledger.getVNames();
  for (const std::string& vname : vnames) {
    if (m_contact_ledger.hasVNameValid(vname)) {
      double vx = m_contact_ledger.getX(vname);
      double vy = m_contact_ledger.getY(vname);
      m_vehicles_sense.push_back(XYPoint(vx, vy));
    }
  }
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SectorSense::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Update which swimmers you should be sensing
  updateSwimmers();

  // Sense swimmers
  std::vector<double> swimmer_sensor_readings = m_swimmer_sensor.query(
    m_swimmers_sense, m_nav_x, m_nav_y, m_nav_hdg
  );
  m_swimmer_readings_str = vectorToStream(swimmer_sensor_readings, ",");

  // Start with swimmer readings for combined sensor readings
  std::vector<double> sensor_readings = swimmer_sensor_readings;

  // Handle all vehicle sensing in one block
  if (m_sense_vehicles) {
    // Update current time for contact ledger
    m_contact_ledger.setCurrTimeUTC(MOOSTime());

    // Update which vehicles you should be sensing
    updateVehicles();

    // Sense vehicles
    std::vector<double> vehicle_sensor_readings = m_vehicle_sensor.query(
      m_vehicles_sense, m_nav_x, m_nav_y, m_nav_hdg
    );
    m_vehicle_readings_str = vectorToStream(vehicle_sensor_readings, ",");

    // Concatenate vehicle readings to sensor readings
    sensor_readings.insert(sensor_readings.end(), vehicle_sensor_readings.begin(), vehicle_sensor_readings.end());
  }

  // Publish combined sensor readings
  m_sensor_readings_str = vectorToStream(sensor_readings, ",");
  Notify("SECTOR_SENSOR_READING", m_sensor_readings_str);

  // Visualize sector readings only if enabled
  if (m_visualize_swim_sectors) {
    std::vector<XYPolygon> polygons = generatePolygons(swimmer_sensor_readings);
    for (const XYPolygon& poly : polygons) {
      std::string spec = poly.get_spec();
      Notify("VIEW_POLYGON", spec);
    }
  }

  // Visualize vehicle sectors if enabled
  if (m_visualize_vehicle_sectors && m_sense_vehicles) {
    std::vector<double> vehicle_sensor_readings = m_vehicle_sensor.query(
      m_vehicles_sense, m_nav_x, m_nav_y, m_nav_hdg
    );
    std::vector<XYPolygon> vehicle_polygons = generateVehiclePolygons(vehicle_sensor_readings);
    for (const XYPolygon& poly : vehicle_polygons) {
      std::string spec = poly.get_spec();
      Notify("VIEW_POLYGON", spec);
    }
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SectorSense::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "sensor_rad") {
      handled = setNonNegDoubleOnString(m_sensor_rad, value);
    }
    else if(param == "saturation_rad") {
      handled = setNonNegDoubleOnString(m_saturation_rad, value);
    }
    else if(param == "num_swimmer_sectors") {
      handled = setIntOnString(m_num_swimmer_sectors, value);
    }
    else if(param == "num_vehicle_sectors") {
      handled = setIntOnString(m_num_vehicle_sectors, value);
    }
    else if(param == "arc_points") {
      handled = setIntOnString(m_arc_points, value);
    }
    else if(param == "visualize_swim_sectors") {
      handled = setBooleanOnString(m_visualize_swim_sectors, value);
    }
    else if(param == "visualize_vehicle_sectors") {
      handled = setBooleanOnString(m_visualize_vehicle_sectors, value);
    }
    else if(param == "sense_vehicles") {
      handled = setBooleanOnString(m_sense_vehicles, value);
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  // Compute sector widths
  m_swim_sector_width = 360.0/m_num_swimmer_sectors;

  // Initialize underlying sensor classes
  m_swimmer_sensor.initialize(
    m_sensor_rad,
    m_saturation_rad,
    m_num_swimmer_sectors,
    NormalizationRule::DYNAMIC
  );

  if (m_sense_vehicles) {
    m_vehicle_sector_width = 360.0/m_num_vehicle_sectors;
    m_vehicle_sensor.initialize(
      m_sensor_rad,
      m_saturation_rad,
      m_num_vehicle_sectors,
      NormalizationRule::DYNAMIC
    );
  }

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void SectorSense::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("SWIMMER_ALERT", 0);
  Register("FOUND_SWIMMER", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_HEADING", 0);

  // Only register for NODE_REPORT if we're sensing vehicles
  if (m_sense_vehicles) {
    Register("NODE_REPORT", 0);
  }
}


//-------------------------------------------------------------
// Procedure: calcDeltaHeading(double heading1, double heading2)
//            the delta in degrees from heading1 to heading2
//            returns the delta heading in degrees [-180, 180]
double SectorSense::calcDeltaHeading(double heading1, double heading2)
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

//------------------------------------------------------------
// Procedure: buildReport()

bool SectorSense::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: SectorSense.cpp                       " << endl;
  m_msgs << "============================================" << endl;

  m_msgs << "m_sensor_readings_str: " << m_sensor_readings_str << endl;
  m_msgs << "m_swimmer_readings_str: " << m_swimmer_readings_str << endl;
  if (m_sense_vehicles) {
    m_msgs << "m_vehicle_readings_str: " << m_vehicle_readings_str << endl;
    m_msgs << "num vehicles_tracked: " << m_contact_ledger.size() << std::endl;
  }
  m_msgs << "num swimmers_logged: " << m_swimmer_map.size() << std::endl;
  m_msgs << "latest node report: " << m_node_report << std::endl;
  m_msgs << "--------------------------------------------" << endl;

  ACTable actab(3);
  actab << " Swimmer Idx | Info | Rescued ";
  actab.addHeaderLines();
  for (const std::pair<int, Swimmer>& entry : m_swimmer_map) {
      int id = entry.first;
      const Swimmer& s = entry.second;
      actab << intToString(id) << s.position.get_spec() << boolToString(s.rescued);
    }

  m_msgs << actab.getFormattedString();

  return(true);
}

// Helper function to create a sector polygon
// arc_points: Number of points for arc smoothness
XYPolygon SectorSense::buildSectorPolygon(double cx, double cy, double heading_deg,
                             double sector_start_deg, double sector_end_deg,
                             double radius, int arc_points)
{
  XYPolygon poly;
  poly.add_vertex(cx, cy); // Center point

  // Adjust angles to be offset by the heading
  // No need to normalize angles back to 0->360 afterwards because
  // sin and cos deal with the wrap-around
  double start_rad = (sector_start_deg-heading_deg) * M_PI / 180.0;
  double end_rad   = (sector_end_deg-heading_deg) * M_PI / 180.0;
  double delta_rad = (end_rad - start_rad) / arc_points;

  // Add points along the arc
  for (int i = 0; i <= arc_points; ++i) {
    double angle = start_rad + i * delta_rad;
    double x = cx + radius * cos(angle);
    double y = cy + radius * sin(angle);
    poly.add_vertex(x, y);
  }
  poly.set_label("sector");
  return poly;
}

// Helper function to process a swimmer alert message

void SectorSense::processSwimmerAlert(CMOOSMsg& msg) {
  // Get the id of the swimmer so we don't double count any swimmers
  std::vector<string> mvector = parseString(msg.GetString(), ",");
  unsigned int vsize = mvector.size();
  for (int i=0; i<vsize; i++) {
    string param = tolower(biteStringX(mvector[i], '='));
    string value = mvector[i];
    if(param == "id") {
      // We got the id. If we don't have this id, add it.
      // Otherwise, don't do anything.
      unsigned int swimmer_id = std::stoi(value);
      if (m_swimmer_map.count(swimmer_id) == 0) {
        XYPoint position = string2Point(msg.GetString());
        Swimmer new_swimmer(position);
        m_swimmer_map[swimmer_id] = new_swimmer;
      }
    }
  }
}

void SectorSense::processFoundSwimmer(CMOOSMsg& msg) {
  // Get the id of the swimmer
  std::vector<string> mvector = parseString(msg.GetString(), ",");
  unsigned int vsize = mvector.size();
  for (int i=0; i<vsize; i++) {
    string param = tolower(biteStringX(mvector[i], '='));
    if(param == "id") {
      // We got the id. If we have this id, mark it as rescued
      // Otherwise, add it and mark it as rescued
      unsigned int swimmer_id = std::stoi(mvector[i]);
      if (m_swimmer_map.count(swimmer_id) > 0) {
        // Mark that this swimmer has been saved
        m_swimmer_map[swimmer_id].rescued = true;
      }
      else {
        m_swimmer_map[swimmer_id] = Swimmer(true);
      }
    }
  }
}

void SectorSense::processVehicleReport(CMOOSMsg& msg) {
  std::string whynot;
  std::string vname = m_contact_ledger.processNodeReport(msg.GetString(), whynot);
  if (vname.empty() && !whynot.empty()) {
    reportRunWarning("Failed to process vehicle report: " + whynot);
  }
}

std::vector<XYPolygon> SectorSense::generatePolygons(std::vector<double> sensor_readings) {
  std::vector<XYPolygon> polygons;
  for(int i=0; i<m_num_swimmer_sectors; ++i) {
    double sector_start = -(i*m_swim_sector_width) + 90.0 - m_swim_sector_width/2.0;
    double sector_end = -(i*m_swim_sector_width) + 90.0 + m_swim_sector_width/2.0;
    XYPolygon poly = buildSectorPolygon(
      m_nav_x, m_nav_y, m_nav_hdg, sector_start, sector_end, m_sensor_rad, m_arc_points
    );
    // Add shading according to the sensor reading
    poly.set_color("edge", ColorPack(0,1,0));
    poly.set_color("fill", ColorPack(0,1,0));
    poly.set_color("vertex", ColorPack(0,1,0));
    poly.set_transparency(sensor_readings[i]*0.5);
    poly.set_label("sector_" + intToString(i));
    poly.set_msg("reading=" + doubleToString(sensor_readings[i], 2));
    polygons.push_back(poly);
  }
  return polygons;
}

std::vector<XYPolygon> SectorSense::generateVehiclePolygons(std::vector<double> sensor_readings) {
  std::vector<XYPolygon> polygons;
  for(int i=0; i<m_num_vehicle_sectors; ++i) {
    double sector_start = -(i*m_vehicle_sector_width) + 90.0 - m_vehicle_sector_width/2.0;
    double sector_end = -(i*m_vehicle_sector_width) + 90.0 + m_vehicle_sector_width/2.0;
    XYPolygon poly = buildSectorPolygon(
      m_nav_x, m_nav_y, m_nav_hdg, sector_start, sector_end, m_sensor_rad, m_arc_points
    );
    // Use different colors for vehicle sectors (blue instead of green)
    poly.set_color("edge", ColorPack(0,0,1));
    poly.set_color("fill", ColorPack(0,0,1));
    poly.set_color("vertex", ColorPack(0,0,1));
    poly.set_transparency(sensor_readings[i]*0.5);
    poly.set_label("vehicle_sector_" + intToString(i));
    poly.set_msg("vehicle_reading=" + doubleToString(sensor_readings[i], 2));
    polygons.push_back(poly);
  }
  return polygons;
}
