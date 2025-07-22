#--------------------------------------------------------
# The CMakeLists.txt for:          lib_bhv_follow_com
# Author(s):                                 E. Gonzalez
#--------------------------------------------------------
#                                      BHV_MaxReading
#--------------------------------------------------------
ADD_LIBRARY(BHV_MaxReading SHARED
  BHV_MaxReading.cpp)

TARGET_INCLUDE_DIRECTORIES(BHV_MaxReading PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(BHV_MaxReading
   helmivp
   behaviors
   ivpbuild
   logic
   ivpcore
   bhvutil
   mbutil
   geometry
   neural_network
   ivp_behavior_extend
   general_utils
)
