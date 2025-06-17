#--------------------------------------------------------
# The CMakeLists.txt for:                       pSectorSense
# Author(s):                              Everardo Gonzalez
#--------------------------------------------------------

SET(SRC
  SectorSense.cpp
  SectorSense_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pSectorSense ${SRC})

TARGET_LINK_LIBRARIES(pSectorSense
  ${MOOS_LIBRARIES}
  geometry
  apputil
  mbutil
  m
  pthread)

