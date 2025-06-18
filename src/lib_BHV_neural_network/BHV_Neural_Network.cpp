/************************************************************/
/*    NAME: Everardo Gonzalez                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Neural_Network.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_Neural_Network.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Neural_Network::BHV_Neural_Network(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
  addInfoVars("SECTOR_SENSOR_READING");

  m_best_heading = 0.0;
  m_best_speed   = 0.0;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Neural_Network::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());

  // TODO initilize the network here

  if((param == "foo") && isNumber(val)) {
    // Set local member variables here
    return(true);
  }
  else if (param == "bar") {
    // return(setBooleanOnString(m_my_bool, val));
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_Neural_Network::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Neural_Network::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Neural_Network::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Neural_Network::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Neural_Network::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Neural_Network::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Neural_Network::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Neural_Network::onRunState()
{

  // Part 1: Get the latest sensor reading from SectorSense
  bool ok_sensor_reading = processSensorReadings();
  if (!ok_sensor_reading)
    return(0);

  // Part 2:
  forwardPropNetwork();

  // Part 3: Build the IvP function
  IvPFunction *ipf = buildFunction();

  return(ipf);
}



//---------------------------------------------------------------
// Procedure: processSensorReadings()
//   Purpose: Get the sensor reading message from the buffer,
//            convert it, and load it into the member variable
//            for sensor data, m_sector_sensor_readings

bool BHV_Neural_Network::processSensorReadings()
{

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
      postWMessage("Bad reading in sensor: i=" + intToString(i)
		   + ", val=" + readings_str[i]);
      return(false);
    }

    m_sector_sensor_readings.push_back(temp_dbl);
  }

  return(true);
}

//---------------------------------------------------------------
// Procedure: forwardPropNetwork()
//   Purpose: Runs a forward prop of the network

void BHV_Neural_Network::forwardPropNetwork()
{
  std::vector<double> outputs = m_network.forward(m_sector_sensor_readings);

  // Map from outputs to heading and speed
  // m_best_speed   = outputs[0] ?
  // m_best_heading = outputs[1] ?

  return;
}


//-----------------------------------------------------------
// Procedure: buildFunction()

IvPFunction* BHV_Neural_Network::buildFunction()
{
  // Part 1: Build the IvP function

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

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}
