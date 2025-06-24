/************************************************************/
/*    NAME: Everardo Gonzalez, Tyler Paine                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_FollowCOM.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_FollowCOM.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_FollowCOM::BHV_FollowCOM(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
  addInfoVars("SECTOR_SENSING_READING");

  m_best_heading = 0.0;
  m_best_speed = 0.2;

  std::cout << "Successfully constructed BHV_FollowCOM" << std::endl;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_FollowCOM::setParam(string param, string val)
{
  // No parameters so always return false
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_FollowCOM::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_FollowCOM::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_FollowCOM::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_FollowCOM::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_FollowCOM::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_FollowCOM::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_FollowCOM::onRunToIdleState()
{
}

bool BHV_FollowCOM::updateHeading() {
  if (m_sector_sensor_readings.size() == 0){
    return(false);
  }

  int number_sectors = m_sector_sensor_readings.size();
  double number_sec_dbl = static_cast<double>(number_sectors);

  // Coordinates of each wieghted reading in body relative coordinates
  std::vector<double> rel_x; // needed?
  std::vector<double> rel_y;
  double sum_x = 0.0;
  double sum_y = 0.0;

  for (int i=0; i<m_sector_sensor_readings.size(); i++){
    // angle for this sensor
    double i_dbl = static_cast<double>(i);
    double angle = 360.0 / number_sec_dbl * i_dbl;
    rel_x.push_back( sin( 3.14 / 180.0 * angle) * m_sector_sensor_readings[i]);
    rel_y.push_back( cos( 3.14 / 180.0 * angle) * m_sector_sensor_readings[i]);
    sum_x += sin( 3.14 / 180.0 * angle) * m_sector_sensor_readings[i];
    sum_y += cos( 3.14 / 180.0 * angle) * m_sector_sensor_readings[i];
  }

  // get the best angle we need to move to by computing the
  // atan of the local weighted inputs
  m_best_heading = atan2(sum_y, sum_x);
  return(true);
}

bool BHV_FollowCOM::processSensorReadings() {
  // Get the var from the buffer
  bool ok = false;
  std::string sensor_input_str = getBufferStringVal("SECTOR_SENSOR_READING", ok);
  if(!ok) {
    postWMessage("No ownship sensor info in info_buffer.");
    return(false);
  }

  // Parse the sensor reading into input for the neural network
  // and update the member variable
  double temp_dbl;
  bool good_reading;

  std::vector<std::string> readings_str = parseString(sensor_input_str, ',');

  m_sector_sensor_readings.clear();
  for (int i=0; i<readings_str.size(); i++){

    good_reading = setDoubleOnString(temp_dbl, readings_str[i]);

    if(!good_reading) {
      postWMessage("Bad reading in index: i=" + intToString(i)
		   + ", val=" + readings_str[i]);
      return(false);
    }

    m_sector_sensor_readings.push_back(temp_dbl);
  }

  return(true);
}
//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_FollowCOM::onRunState()
{
  postEventMessage("Running onRunState()");

  // Part 1: Get the latest sensor reading from SectorSense
  bool ok_sensor_reading = processSensorReadings();
  if (!ok_sensor_reading)
    return(0);
  postEventMessage("Got the sensor readings.");

  // Part 2: Run the heading calculation
  if (!updateHeading()) {
    return(0);
  }
  postEventMessage("Ran heading calculation.");

  // Part 3: Build the IvP function
  IvPFunction *ipf = buildFunction();
  postEventMessage("Built the IvP function.");

  return(ipf);
}

IvPFunction* BHV_FollowCOM::buildFunction() {
  // Assemble function for course (heading)
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_best_heading);
  crs_zaic.setPeakWidth(10);
  crs_zaic.setBaseWidth(10);
  crs_zaic.setMinMaxUtil(20, 100);
  crs_zaic.setSummitDelta(60);
  crs_zaic.setValueWrap(true);
  if(crs_zaic.stateOK() == false) {
    string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }

  // Assemble function for speed
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(m_best_speed);
  spd_zaic.setPeakWidth(0.1);
  spd_zaic.setBaseWidth(0.1);
  spd_zaic.setMinMaxUtil(20, 100);
  spd_zaic.setSummitDelta(60);
  if(spd_zaic.stateOK() == false) {
    string warnings = "Speed ZAIC problems " + spd_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }

  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();

  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(crs_ipf, spd_ipf, 50, 50);

  // Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}
