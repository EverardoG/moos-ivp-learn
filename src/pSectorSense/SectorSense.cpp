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
  for (int i = 0; i < m_swimmers.size(); i++) {
    if (!m_swimmers_rescued[i]) {
      m_swimmers_sense.push_back(m_swimmers[i]);
    }
  }
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SectorSense::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  // This is where we need to send outgoing mail
  // for what SectorSense is generating as readings

  // Update which swimmers you should be sensing
  updateSwimmers();

  // Sense those swimmers!
  std::vector<double> sensor_readings = m_swimmer_sensor.query(
    m_swimmers_sense, m_nav_x, m_nav_y, m_nav_hdg
  );
  m_sensor_readings_str = vectorToStream(sensor_readings, ",");
  Notify("SECTOR_SENSOR_READING", m_sensor_readings_str);

  // Visualize sector readings
  std::vector<XYPolygon> polygons = generatePolygons(sensor_readings);
  for (const XYPolygon& poly : polygons) {
    std::string spec = poly.get_spec();
    Notify("VIEW_POLYGON", spec);
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
    else if(param == "number_sectors") {
      handled = setIntOnString(m_number_sectors, value);
    }
    else if(param == "arc_points") {
      handled = setIntOnString(m_arc_points, value);
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  // Compute sector width
  m_sector_width = 360.0/m_number_sectors;

  // Initialize underlying sensor class
  m_swimmer_sensor.initialize(
    m_sensor_rad,
    m_saturation_rad,
    m_number_sectors,
    NormalizationRule::DYNAMIC
  );

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void SectorSense::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("SWIMMER_ALERT", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_HEADING", 0);

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
  m_msgs << "num swimmers: " << m_swimmers.size() << std::endl;
  m_msgs << "m_saturation_rad: " << m_saturation_rad << std::endl;

  ACTable actab(3);
  actab << " Swimmer Idx | Info | Rescued ";
  actab.addHeaderLines();
  for (int i=0; i<m_swimmers.size(); i++){
    actab << intToString(i) << m_swimmers[i].get_spec()
	  << boolToString(m_swimmers_rescued[i]) ;
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
  std::cout << msg.GetString() << std::endl;
}

void SectorSense::processFoundSwimmer(CMOOSMsg& msg) {
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
      if (m_swimmers_recorded.count(swimmer_id) > 0) {
        // Mark that this swimmer has been saved
        m_swimmer_map[swimmer_id].rescued = true;
      }
    }
  }
  std::cout << msg.GetString() << std::endl;
}

std::vector<XYPolygon> SectorSense::generatePolygons(std::vector<double> sensor_readings) {
  std::vector<XYPolygon> polygons;
  for(int i=0; i<m_number_sectors; ++i) {
    double sector_start = -(i*m_sector_width) + 90.0 - m_sector_width/2.0;
    double sector_end = -(i*m_sector_width) + 90.0 + m_sector_width/2.0;
    XYPolygon poly = buildSectorPolygon(
      m_nav_x, m_nav_y, m_nav_hdg, sector_start, sector_end, m_sensor_rad, m_arc_points
    );
    // Add shading according to the sensor reading
    double reading = 255.0*sensor_readings[i];
    poly.set_color("edge", ColorPack(0,1,0));
    poly.set_color("fill", ColorPack(0,1,0));
    poly.set_color("vertex", ColorPack(0,1,0));
    poly.set_transparency(sensor_readings[i]*0.5);
    poly.set_label("sector_" + intToString(i));
    poly.set_msg("reading=" + doubleToString(reading, 2));
    polygons.push_back(poly);
  }
  return polygons;
}
