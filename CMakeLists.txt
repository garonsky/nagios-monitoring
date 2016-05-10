project (hpccsystems-nagios-monitoring)
cmake_minimum_required (VERSION 3.5.1)

set (HPCC_NM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE ("************ Setting CMAKE_MODULE_PATH to ${HPCC_NM_SOURCE_DIR}/cmake_modules/")
set (CMAKE_MODULE_PATH "${HPCC_NM_SOURCE_DIR}/cmake_modules/")

MESSAGE ("CMAKE_MODULE_PATH is set to ${CMAKE_MODULE_PATH}")

###
## Build Level
###
if( NOT BUILD_LEVEL )
    set ( BUILD_LEVEL "COMMUNITY" )
endif()
###

set( BUILD_TAG "${projname}_${version}-${stagever}")

set (HPCC_SOURCE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

IF ("${HPCC_SOURCE_DIR}" STREQUAL "")
  SET (HPCC_SOURCE_DIR ${HPCC_SOURCE_ROOT_DIR}/HPCC-Platform)
ENDIF()

include (${HPCC_NM_SOURCE_DIR}/version.cmake)
include (${HPCC_SOURCE_DIR}/version.cmake)
include (${HPCC_SOURCE_DIR}/cmake_modules/optionDefaults.cmake)
include (${HPCC_SOURCE_DIR}/cmake_modules/commonSetup.cmake)
include (${HPCC_SOURCE_DIR}/version.cmake)

MESSAGE (HPCC_SOURCE_DIR: ${HPCC_SOURCE_DIR})
ADD_SUBDIRECTORY(${HPCC_SOURCE_DIR} oss EXCLUDE_FROM_ALL)
INCLUDE(${HPCC_NM_SOURCE_DIR}/platform-version-prereq.cmake)

CONFIGURE_FILE(${HPCC_SOURCE_DIR}/build-config.h.cmake "build-config.h" )

HPCC_ADD_SUBDIRECTORY( hpcc-nagios-tools )
HPCC_ADD_SUBDIRECTORY( utils )
HPCC_ADD_SUBDIRECTORY( config )
HPCC_ADD_SUBDIRECTORY( commands )

MESSAGE (HPCC_SOURCE_DIR: ${HPCC_SOURCE_DIR})
MESSAGE ( "HPCC_NM_SOURCE_DIR ${HPCC_NM_SOURCE_DIR}" )
