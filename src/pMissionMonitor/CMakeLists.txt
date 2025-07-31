#--------------------------------------------------------
# The CMakeLists.txt for:                 pMissionMonitor
# Author(s):                              Manuel Agraz
#--------------------------------------------------------

SET(SRC
  MissionMonitor.cpp
  MissionMonitor_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pMissionMonitor ${SRC})

TARGET_LINK_LIBRARIES(pMissionMonitor
  ${MOOS_LIBRARIES}
  geometry
  apputil
  mbutil
  m
  pthread
  general_utils)

