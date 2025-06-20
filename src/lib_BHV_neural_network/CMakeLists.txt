#--------------------------------------------------------
# The CMakeLists.txt for:          lib_bhv_neural_network
# Author(s):                                 E. Gonzalez
#--------------------------------------------------------
#                                      BHV_neural_network
#--------------------------------------------------------
ADD_LIBRARY(BHV_Neural_Network SHARED
  BHV_Neural_Network.cpp)

TARGET_INCLUDE_DIRECTORIES(BHV_Neural_Network PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(BHV_Neural_Network
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
)
