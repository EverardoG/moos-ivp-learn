#--------------------------------------------------------
# The CMakeLists.txt for:          ivp_behavior_extend
# Author(s):                                 E. Gonzalez
#--------------------------------------------------------
#                                   ivp_behavior_extend
#--------------------------------------------------------
add_library(ivp_behavior_extend ivp_behavior_extend.cpp)
target_link_libraries(ivp_behavior_extend
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
