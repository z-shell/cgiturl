cmake_minimum_required( VERSION 2.8.12 )
project( cgiturl )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" )

include( FindPkgConfig )
include( compiler_req )
include( use_cxx11 )

compiler_req()      # test required compiler versions
use_cxx11()         # configure c++11 usage

# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )

#
# Follow source files, headers
#

SET( SRCS
    main.cpp
    coding_functions.cpp
    math_functions.cpp
    util.cpp
)


if(NOT DEFINED ENV{GITURL_NO_CGITURL})
    add_executable( cgiturl ${SRCS} )
endif()

if(NOT DEFINED ENV{GITURL_NO_CGITURL})
    install( TARGETS cgiturl RUNTIME DESTINATION bin )
endif()

add_custom_target( uninstall "${CMAKE_COMMAND}" -P "${CMAKE_MODULE_PATH}/uninstall.cmake" )
