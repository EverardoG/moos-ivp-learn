##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
#              2020/05/09 minor mods
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(lib_behaviors-test)
ADD_SUBDIRECTORY(pExampleApp)
ADD_SUBDIRECTORY(pXRelayTest)

ADD_SUBDIRECTORY(lib_BHV_neural_network)
ADD_SUBDIRECTORY(lib_BHV_follow_com)
ADD_SUBDIRECTORY(pSectorSense)
ADD_SUBDIRECTORY(uFldRecordKeeper)
ADD_SUBDIRECTORY(neural_network)
ADD_SUBDIRECTORY(sector_sensor)
ADD_SUBDIRECTORY(general_utils)
ADD_SUBDIRECTORY(ivp_behavior_extend)
ADD_SUBDIRECTORY(pSimpleControl)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

