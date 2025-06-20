/************************************************************/
/*    NAME: Everardo Gonzalez                               */
/*    ORGN: MIT                                             */
/*    FILE: ivp_behavior_extend.cpp                          */
/*    DATE:                                                 */
/************************************************************/

#include "ivp_behavior_extend.h"

bool IvPBehaviorExtend::setVecDoubleOnString(std::vector<double> given_vec_double, const std::string& str) {
  double temp_dbl;
  bool good_reading;

  // Turn our string into a vector of strings, using the comma as a delimiter
  std::vector<std::string> vec_str = parseString(str, ',');

  for (int i=0; i<vec_str.size(); i++){
    // Turn string into a double
    good_reading = setDoubleOnString(temp_dbl, vec_str[i]);

    if(!good_reading) {
      postWMessage("Bad reading in index: i=" + intToString(i)
		   + ", val=" + vec_str[i]);
      return(false);
    }

    given_vec_double.push_back(temp_dbl);
  }
  return true;
}

bool IvPBehaviorExtend::setVecIntOnString(std::vector<int> given_vec_int, const std::string& str) {
  int temp_int;
  bool good_reading;

  // Turn our string into a vector of strings, using the comma as a delimiter
  std::vector<std::string> vec_str = parseString(str, ',');

  for (int i=0; i<vec_str.size(); i++){
    // Turn string into an int
    good_reading = setIntOnString(temp_int, vec_str[i]);

    if(!good_reading) {
      postWMessage("Bad reading in index: i=" + intToString(i)
		   + ", val=" + vec_str[i]);
      return(false);
    }

    given_vec_int.push_back(temp_int);
  }
  return true;
}
