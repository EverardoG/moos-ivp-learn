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
  addInfoVars("NAV_HEADING");
  addInfoVars("SECTOR_SENSOR_READING");

  m_best_delta_heading = 0.0;
  m_best_speed = 0.2;

  std::cout << "Successfully constructed BHV_FollowCOM" << std::endl;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_FollowCOM::setParam(string param, string val)
{
  param = tolower(param);
  double double_val = atof(val.c_str());
  if ((param == "speed") && (isNumber(val))) {
    m_best_speed = double_val;
    return(true);
  }
  // unrecognized parameter
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
  if (m_sector_sensor_readings.size() == 0) {
    return(false);
  }
  std::vector<XYPoint> readings_pts;
  for (int i = 0; i < m_sector_sensor_readings.size(); i++) {
    readings_pts.emplace_back(XYPoint(readingToXY(m_sector_sensor_readings.size(), i, m_sector_sensor_readings[i])));
  }
  XYPoint sum_pt = sumXY(readings_pts);
  m_best_delta_heading = XYToRelAngle(sum_pt);
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
  postEventMessage("Running onRunState() in FollowCOM");

  // Part 1: Get the latest sensor reading from SectorSense
  bool ok_sensor_reading = processSensorReadings();
  if (!ok_sensor_reading)
    return(0);
  postEventMessage("Got the sensor readings.");

  // Part 1a: Get the heading
  bool ok_heading = processHeading();
  if (!ok_heading)
    return(0);
  postEventMessage("Got the heading.");


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
  crs_zaic.setSummit(angle360(m_best_delta_heading+m_nav_heading));
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

bool BHV_FollowCOM::processHeading()
{
  bool ok = false;
  m_nav_heading = getBufferDoubleVal("NAV_HEADING", ok);
  if (!ok) {
    postWMessage("No ownship sensor info in info_buffer.");
    return(false);
  }
  return(true);
}
