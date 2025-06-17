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

  m_nav_x          = 0.0;
  m_nav_y          = 0.0;
  m_nav_hdg        = 0.0;
  m_sensor_rad     = 10.0;
  m_number_sectors = 8; 
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

    if(key == "SWIMMER_ALEART") {
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

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SectorSense::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: fillSensorBuckets()
//            updates

void SectorSense::fillSensorBuckets()
{
  for (int i=0; i<m_swimmers.size(); i++){

    // How far away is this swimmer?
    double dx = m_nav_x - m_swimmers[i].get_vx();
    double dy = m_nav_y - m_swimmers[i].get_vy();
    double dist = sqrt(dx*dx + dy*dy);

    if (dist > m_sensor_rad)
      continue;

    // What is the relative heading of this swimmer?
    double rel_angle = relAng(m_nav_x, m_nav_y,
				m_swimmers[i].get_vx(), m_swimmers[i].get_vy());

    
    // convert to local angle from straight ahead
    double angle_delta = calcDeltaHeading(m_nav_hdg, rel_angle); 

    // Determine which bucket this is in from straight ahead.
    // Bucket 0 is from straigh ahead to sector_width to the right
    double sector_width = 360.0 / ((double) m_number_sectors);
    double angle_pos = angle_delta / sector_width;
    
    int j = 0;     // temp counter
    int k = 0;     // bucket number
    if ( angle_pos > 0){
      j = std::floor(angle_pos); // j = 0, 1, 2 ... (m_number_sectors/2-2)
      k = j; 
    } else {
      j = std::floor( -1.0 * angle_pos);
      j *= -1;                   // j = 0, -1, -2 ... (-m_number_sectors/2+2)
      k = m_number_sectors - 1 + j; // k = (m_number_sectors -1), (m_number_sectors -2),
                                    //      ... (m_number_sectors/2 -1)
    }

    
  }

  return; 
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
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  
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




