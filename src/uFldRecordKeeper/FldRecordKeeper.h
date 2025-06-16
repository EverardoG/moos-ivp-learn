/************************************************************/
/*    NAME: Everardo Gonzalez                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: FldRecordKeeper.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef FldRecordKeeper_HEADER
#define FldRecordKeeper_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class FldRecordKeeper : public AppCastingMOOSApp
{
 public:
   FldRecordKeeper();
   ~FldRecordKeeper();

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

 private: // State variables
};

#endif 
