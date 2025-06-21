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

using namespace std;

//---------------------------------------------------------
// Constructor()

SectorSense::SectorSense()
{
  // These variables change
  m_nav_x          = 0.0;
  m_nav_y          = 0.0;
  m_nav_hdg        = 0.0;

  // These variables are fixed
  m_sensor_rad     = 100.0;
  m_saturation_rad = 0.1;
  m_number_sectors = 8;
  m_sector_width = 360.0/m_number_sectors;

  m_swimmer_sensor.initialize(
    m_sensor_rad,
    m_saturation_rad,
    m_number_sectors,
    NormalizationRule::DYNAMIC
  );
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
      XYPoint new_point = string2Point(msg.GetString());
      m_swimmers.push_back(new_point);
      m_swimmers_rescued.push_back(false);

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
    else if(param == "number_sectors") {
      handled = setIntOnString(m_number_sectors, value);
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  // initialize sensor buckets.
  if (m_number_sectors > 0){
    for (int i=0; i<m_number_sectors; i++){
      m_sensor_buckets.push_back(0.0);
    }
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




