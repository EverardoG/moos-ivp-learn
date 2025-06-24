#--------------------------------------------------------
# The CMakeLists.txt for:          lib_bhv_follow_com
# Author(s):                                 E. Gonzalez
#--------------------------------------------------------
#                                      BHV_FollowCOM
#--------------------------------------------------------
ADD_LIBRARY(BHV_FollowCOM SHARED
  BHV_FollowCOM.cpp)

TARGET_INCLUDE_DIRECTORIES(BHV_FollowCOM PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(BHV_FollowCOM
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
