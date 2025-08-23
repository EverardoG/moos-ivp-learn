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
#include "FileBuffer.h"

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
  addInfoVars("NAV_HEADING");
  addInfoVars("SECTOR_SENSOR_READING");

  m_best_delta_heading = 0.0;
  m_best_speed   = 0.0;

  // Initialize new member variables
  m_swimmer_sectors = 0;
  m_vehicle_sectors = 0;
  m_sense_vehicles = false;
  m_expected_size = 0;
  m_initialization_failed = false;

  std::cout << "Successfully constructed BHV_Neural_Network" << std::endl;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Neural_Network::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());

  // Initialize network using csv file
  if (param == "csv_directory") {
    m_csv_directory = val;
    return(true);
  }

  if (param == "swimmer_sectors") {
    m_swimmer_sectors = (int)double_val;
    return(true);
  }

  if (param == "vehicle_sectors") {
    m_vehicle_sectors = (int)double_val;
    return(true);
  }

  if (param == "sense_vehicles") {
    return setBooleanOnString(m_sense_vehicles, val);
  }

  // We don't know what this parameter is. Return false
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_Neural_Network::onSetParamComplete()
{
  // Calculate expected sensor reading size
  m_expected_size = m_swimmer_sectors;
  if (m_sense_vehicles) {
    m_expected_size += m_vehicle_sectors;
  }

  std::string vec_str("");

  // use FileBuffer() to read in config file
  // Assume the config file is in the mission directory
  //    (not the top level missions directory, but the directory of a particular mission)
  vector<string> lines = fileBuffer(m_csv_directory);
  if(lines.size() == 0) {
    postWMessage("File not found, or empty: " + m_csv_directory);
    m_initialization_failed = true;
    return;
  }
  postEventMessage("Csv file found.");

  // Use the extension method to populate a vector of doubles
  // from line 0. These are the weights for the neural network
  std::vector<double> weights;
  string warning;
  bool good_reading;
  good_reading = setVecDoubleOnString(weights, lines[0], warning);
  if(!good_reading) {
    postWMessage("Failed to read neural network weights. Bad reading for line 0 of file: " + m_csv_directory + ". " + warning);
    m_initialization_failed = true;
    return;
  }
  vec_str="";
  for (double val : weights) {
    vec_str = vec_str + std::to_string(val) + " ";
  }
  postEventMessage("Successfully read in weights: "+vec_str);

  // Use the extension method to populate a vector of ints
  // from line 1. These define the structure of the neural network
  std::vector<int> structure;
  warning = "";
  good_reading = setVecIntOnString(structure, lines[1], warning);
  if(!good_reading) {
    postWMessage("Failed to read neural network structure. Bad reading for line 1 of file: " + m_csv_directory + ". " + warning);
    m_initialization_failed = true;
    return;
  }
  vec_str="";
  for (double val : structure) {
    vec_str = vec_str + std::to_string(val) + " ";
  }
  postEventMessage("Successfully read in structure: "+vec_str);

  // Validate that expected sensor size matches network input layer size
  if (structure.size() > 0 && structure[0] != m_expected_size) {
    postWMessage("Network input size mismatch. Network expects " + intToString(structure[0]) +
                 " inputs, but sensor configuration expects " + intToString(m_expected_size) +
                 " (swimmer_sectors=" + intToString(m_swimmer_sectors) +
                 ", vehicle_sectors=" + intToString(m_vehicle_sectors) +
                 ", sense_vehicles=" + (m_sense_vehicles ? "true" : "false") + ")");
    m_initialization_failed = true;
    return;
  }

  // Get the output bounds of the neural network from
  // line 2. These define the output boundaries of the network
  std::vector<double> bounds_flat;
  good_reading = setVecDoubleOnString(bounds_flat, lines[2], warning);
  warning = "";
  if(!good_reading) {
    postWMessage("Failed to read neural network bounds. Bad reading for line 2 of file: " + m_csv_directory + ". " + warning);
    m_initialization_failed = true;
    return;
  }
  vec_str="";
  for (double val : bounds_flat) {
    vec_str = vec_str + std::to_string(val) + " ";
  }
  postEventMessage("Successfully read in bounds: "+vec_str);

  vec_str="";
  for (double val : bounds_flat) {
    vec_str = vec_str + std::to_string(val) + " ";
  }
  postEventMessage("Reshaping vec {"+vec_str+"} into size ("+std::to_string(structure.back())+","+"2)");

  // Turn those output bounds into the correct shape for the network
  std::vector<std::vector<double>> bounds = reshapeVector2D(bounds_flat, structure.back(), 2);

  // Then load that into the neural network
  std::string err("");
  if (!m_network.initialize(weights, structure, bounds, err)) {
    postEMessage("Neural Network failed to initialize. "+err);
    m_initialization_failed = true;
    return;
  }

  // Mark that we have successfully loaded in our network
  m_network_loaded = true;
  postEventMessage("Successfully initialized neural network.");
}

bool BHV_Neural_Network::initialize()
{
  if (m_initialization_failed) {
    postWMessage("Cannot initialize - previous initialization failure detected.");
    return false;
  }
  return m_network_loaded;
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
  postEventMessage("Running onRunState() in NeuralNetwork");

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

  // Part 2:
  if (!initialize()) {
    return(0);
  }

  forwardPropNetwork();
  postEventMessage("Ran forward propogation on neural network. Velocity: "+std::to_string(m_best_speed)+" | Heading: "+std::to_string(m_best_delta_heading));

  // Part 3: Build the IvP function
  IvPFunction *ipf = buildFunction();
  postEventMessage("Built the IvP function.");

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
      postWMessage("Bad reading in index: i=" + intToString(i)
		   + ", val=" + readings_str[i]);
      return(false);
    }

    m_sector_sensor_readings.push_back(temp_dbl);
  }

  // Validate sensor reading size
  if (m_sector_sensor_readings.size() != m_expected_size) {
    postWMessage("Sensor reading size mismatch. Expected: " + intToString(m_expected_size) +
                 " (swimmer_sectors=" + intToString(m_swimmer_sectors) +
                 ", vehicle_sectors=" + intToString(m_vehicle_sectors) +
                 ", sense_vehicles=" + (m_sense_vehicles ? "true" : "false") +
                 "), but received: " + intToString(m_sector_sensor_readings.size()));
    return(false);
  }

  return(true);
}

bool BHV_Neural_Network::processHeading()
{
  bool ok = false;
  m_nav_heading = getBufferDoubleVal("NAV_HEADING", ok);
  if (!ok) {
    postWMessage("No ownship sensor info in info_buffer.");
    return(false);
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
  m_best_speed   = outputs[0];
  m_best_delta_heading = outputs[1];

  return;
}


//-----------------------------------------------------------
// Procedure: buildFunction()

IvPFunction* BHV_Neural_Network::buildFunction()
{
  // Part 1: Build the IvP function

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

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}
