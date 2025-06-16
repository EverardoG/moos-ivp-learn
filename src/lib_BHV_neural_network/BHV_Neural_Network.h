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

protected: // Local Utility functions

protected: // Configuration parameters

protected: // State variables
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_Neural_Network(domain);}
}
#endif
