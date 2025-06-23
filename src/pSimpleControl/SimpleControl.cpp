/************************************************************/
/*    NAME: Tyler Paine                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: SimpleControl.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "SimpleControl.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

SimpleControl::SimpleControl()
{
  m_rud_gain = 0.1;
  m_const_thrust = 20; 
}

//---------------------------------------------------------
// Destructor

SimpleControl::~SimpleControl()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool SimpleControl::OnNewMail(MOOSMSG_LIST &NewMail)
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

    if(key == "SECTOR_SENSOR_READING") {
      std::vector<std::string> sensor_str;
      sensor_str = parseString(msg.GetString(), ',');
      m_sensor.clear();
      for (int i=0; i<sensor_str.size(); i++){
	
	double temp_dbl;
	if (!setDoubleOnString(temp_dbl, msg.GetString())){
	    reportRunWarning("Unhandled Mail: " + key + " : " + msg.GetString());
	    break; 
	}	
	m_sensor.push_back(temp_dbl); 
      }

    } else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool SimpleControl::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SimpleControl::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if (m_sensor.size() == 0){
    AppCastingMOOSApp::PostReport();
    return(true);
  }

  int number_sectors = m_sensor.size();
  double number_sec_dbl = static_cast<double>(number_sectors);

  // Coordinates of each wieghted reading in body relative coordinates
  std::vector<double> rel_x; // needed?
  std::vector<double> rel_y;
  double sum_x = 0.0;
  double sum_y = 0.0; 
  
  for (int i=0; i<m_sensor.size(); i++){
    // angle for this sensor
    double i_dbl = static_cast<double>(i); 
    double angle = 360.0 / number_sec_dbl * i_dbl;
    rel_x.push_back( sin( 3.14 / 180.0 * angle) * m_sensor[i]);
    rel_y.push_back( cos( 3.14 / 180.0 * angle) * m_sensor[i]);
    sum_x += sin( 3.14 / 180.0 * angle) * m_sensor[i];
    sum_y += cos( 3.14 / 180.0 * angle) * m_sensor[i]; 
  }

  // get the best angle we need to move to by computing the
  // atan of the local weighted inputs 
  double best_rel_angle = atan2(sum_y, sum_x);
  double desired_rudder = m_rud_gain * best_rel_angle; // Might need a negative sign?

  // Post it
  Notify("DESIRED_RUDDER", desired_rudder);
  Notify("DESIRED_THRUST", m_const_thrust);  
  
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SimpleControl::OnStartUp()
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
    if(param == "rudder_gain") {
      handled = setPosDoubleOnString(m_rud_gain, value);
    }
    else if(param == "constant_thrust") {
      handled = setPosDoubleOnString(m_const_thrust, value);
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void SimpleControl::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("SECTOR_SENSOR_READING", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool SimpleControl::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}




