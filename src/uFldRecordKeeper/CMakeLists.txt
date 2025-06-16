#--------------------------------------------------------
# The CMakeLists.txt for:                       uFldRecordKeeper
# Author(s):                              Everardo Gonzalez
#--------------------------------------------------------

SET(SRC
  FldRecordKeeper.cpp
  FldRecordKeeper_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldRecordKeeper ${SRC})

TARGET_LINK_LIBRARIES(uFldRecordKeeper
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

