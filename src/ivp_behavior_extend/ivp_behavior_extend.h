/************************************************************/
/*    NAME: Everardo Gonzalez                               */
/*    ORGN: MIT                                             */
/*    FILE: ivp_behavior_extend.h                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef IvP_Behavior_Extend_HEADER
#define IvP_Behavior_Extend_HEADER

#include <string>
#include "IvPBehavior.h"
#include "network.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "AngleUtils.h"
#include "GeomUtils.h"


// Class that extends IvPBehavior to include additional helper functions
// as class methods
class IvPBehaviorExtend : public IvPBehavior {
public:
    IvPBehaviorExtend(IvPDomain domain) : IvPBehavior(domain) {}
    virtual ~IvPBehaviorExtend() {}

    // New methods
    bool setVecDoubleOnString(std::vector<double> given_vec_double, const std::string& str);
    bool setVecIntOnString(std::vector<int> given_vec_int, const std::string& str);
};

#endif