################################################################################
#    HPCC SYSTEMS software Copyright (C) 2014 HPCC Systems.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
################################################################################


# Component: send_status
#####################################################
# Description:
# ------------
#    Cmake send_status
#####################################################

project( send_status )

set (   SRCS
        ${ESPSCM_GENERATED_DIR}/ws_machine_esp.cpp 
        main.cpp
    )

include_directories (
        ${HPCC_SOURCE_DIR}/common
        ${HPCC_SOURCE_DIR}/system/jlib
        ${HPCC_SOURCE_DIR}/system/include
        ${HPCC_SOURCE_DIR}/esp/services
        ${HPCC_SOURCE_DIR}/esp/platform
        ${HPCC_SOURCE_DIR}/esp/bindings
        ${HPCC_SOURCE_DIR}/esp/bindings/SOAP/xpp
        ${HPCC_SOURCE_DIR}/esp/smc/SMCLib
        ${HPCC_SOURCE_DIR}/system/security/shared
        ${HPCC_SOURCE_DIR}/system/xmllib
        ${CMAKE_BINARY_DIR}/oss
        #${CMAKE_BINARY_DIR}
    )

ADD_DEFINITIONS( -D_CONSOLE )

HPCC_ADD_EXECUTABLE( send_status ${SRCS} )
install ( TARGETS send_status RUNTIME DESTINATION ${EXEC_DIR} )
target_link_libraries ( send_status
         jlib
	 SMCLib
    )