#--------------------------------------------------------
# The CMakeLists.txt for:          ivp_behavior_extend
# Author(s):                                 E. Gonzalez
#--------------------------------------------------------
#                                   ivp_behavior_extend
#--------------------------------------------------------
ADD_LIBRARY(ivp_behavior_extend SHARED
  ivp_behavior_extend.cpp)
TARGET_LINK_LIBRARIES(ivp_behavior_extend
   helmivp
   behaviors
   ivpbuild
   logic
   ivpcore
   bhvutil
   mbutil
   geometry
   neural_network
)
