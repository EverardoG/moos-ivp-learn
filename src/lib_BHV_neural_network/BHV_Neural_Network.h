/************************************************************/
/*    NAME: Everardo Gonzalez                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Neural_Network.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef Neural_Network_HEADER
#define Neural_Network_HEADER

#include <string>
#include "IvPBehavior.h"
#include "network.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "general_utils.h"

// This needs to read in a file containing parameters/structure
//     Parameters is a comma-seperated list of precise doubles
//     Structure is the number of inputs (sensor readings), number of units in each intermediate layer, number of outputs (action-space, generally size 2)
// This needs to create the network
// Load in weights that were read
// This needs to do a forward pass and output the actions (absolute velocity and relative heading)
//     velocity action is irrespective of current velocity. Velocity of 1.0 means change the velocity to 1.0, not add 1.0 to current velocity.
//     heading is relative to current heading. Relative heading of +0.5 means add 0.5 to current heading.

class BHV_Neural_Network : public IvPBehavior {
public:
  BHV_Neural_Network(IvPDomain);
  ~BHV_Neural_Network() {};

  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();
  bool         processSensorReadings();
  void         forwardPropNetwork();
  IvPFunction* buildFunction();
  bool         initialize();

protected: // Local Utility functions

protected: // Configuration parameters
  std::string m_csv_directory;

protected: // State variables
  NeuralNetwork m_network;
  bool m_network_loaded = false;

  std::vector<double>  m_sector_sensor_readings;

  double m_best_heading;   // These will hold the outputs
  double m_best_speed;     // for now.

};



#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain)
  {return new BHV_Neural_Network(domain);}
}
#endif
