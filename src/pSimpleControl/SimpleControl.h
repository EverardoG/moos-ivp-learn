/************************************************************/
/*    NAME: Tyler Paine                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: SimpleControl.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef SimpleControl_HEADER
#define SimpleControl_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class SimpleControl : public AppCastingMOOSApp
{
 public:
   SimpleControl();
   ~SimpleControl();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
   double m_rud_gain;
   double m_const_thrust; 

 private: // State variables
   std::vector<double> m_sensor; 
};

#endif 
