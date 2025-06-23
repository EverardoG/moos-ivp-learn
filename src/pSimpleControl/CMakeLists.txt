#--------------------------------------------------------
# The CMakeLists.txt for:                       pSimpleControl
# Author(s):                              Tyler Paine
#--------------------------------------------------------

SET(SRC
  SimpleControl.cpp
  SimpleControl_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pSimpleControl ${SRC})

TARGET_LINK_LIBRARIES(pSimpleControl
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

