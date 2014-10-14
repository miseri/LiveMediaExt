cmake_minimum_required(VERSION 2.6)

PROJECT( LiveMediaExt )

message("Source Dir:" ${LiveMediaExt_SOURCE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${LiveMediaExt_SOURCE_DIR}/../bin)
SET(LIBRARY_OUTPUT_PATH ${LiveMediaExt_SOURCE_DIR}/../lib)
SET(RUNTIME_OUTPUT_DIRECTORY ${LiveMediaExt_SOURCE_DIR}/../bin)

SET(LiveMediaExt_BIN ${LiveMediaExt_SOURCE_DIR}/../bin)

# set this to true to add the externals/live555-CMake projects to the solution
SET(BUILD_LIVE555 true)
# set this to true to add the externals/glog projects to the solution
SET(BUILD_GLOG true)

message("LiveMediaExt_SOURCE_DIR directories:" ${LiveMediaExt_SOURCE_DIR})
#message("GLOG_DIR directories:" $ENV{GLOG_DIR})
message("BOOST_INCLUDE_DIR:" $ENV{BOOST_INCLUDE_DIR})

IF(WIN32)
IF(MSVC12)
message("Detected MSVC12 compiler")
set(MSVC_VER VC12)
ELSEIF(MSVC11)
message("Detected MSVC11 compiler")
set(MSVC_VER VC11)
ELSEIF(MSVC10)
message("Detected MSVC10 compiler")
set(MSVC_VER VC10)
ELSE(MSVC12)
message("WARNING: Unknown/unsupported MSVC version")
ENDIF(MSVC12)
ENDIF(WIN32)

# only for windows.
IF(WIN32)
#add_library( glog UNKNOWN IMPORTED )
#add_library( liveMedia UNKNOWN IMPORTED )
#add_library( BasicUsageEnvironment UNKNOWN IMPORTED )
#add_library( groupsock UNKNOWN IMPORTED )
#add_library( UsageEnvironment UNKNOWN IMPORTED )

# Define two import-locations: one for debug and one for release.
#set_target_properties( glog PROPERTIES IMPORTED_LOCATION_DEBUG ${LiveMediaExt_SOURCE_DIR}/../externals/glog/vsprojects/libglog/VC12/Debug/libglog.lib )
#set_target_properties( glog PROPERTIES IMPORTED_LOCATION_RELEASE ${LiveMediaExt_SOURCE_DIR}/../externals/glog/vsprojects/libglog/VC12/Release/libglog.lib )
#set_target_properties( BasicUsageEnvironment PROPERTIES IMPORTED_LOCATION_DEBUG ${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/lib/Debug/BasicUsageEnvironment.lib )
#set_target_properties( liveMedia PROPERTIES IMPORTED_LOCATION_DEBUG ${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/lib/Debug/liveMedia.lib )
#set_target_properties( groupsock PROPERTIES IMPORTED_LOCATION_DEBUG ${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/lib/Debug/groupsock.lib )
#set_target_properties( UsageEnvironment PROPERTIES IMPORTED_LOCATION_DEBUG ${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/lib/Debug/UsageEnvironment.lib )
#set_target_properties( BasicUsageEnvironment PROPERTIES IMPORTED_LOCATION_RELEASE ${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/lib/Release/BasicUsageEnvironment.lib )
#set_target_properties( liveMedia PROPERTIES IMPORTED_LOCATION_RELEASE ${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/lib/Release/liveMedia.lib )
#set_target_properties( groupsock PROPERTIES IMPORTED_LOCATION_RELEASE ${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/lib/Release/groupsock.lib )
#set_target_properties( UsageEnvironment PROPERTIES IMPORTED_LOCATION_RELEASE ${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/lib/Release/UsageEnvironment.lib )
ENDIF(WIN32)

SET(GLOG_INCLUDE_DIR
${LiveMediaExt_SOURCE_DIR}/../externals/glog
${LiveMediaExt_SOURCE_DIR}/../externals/glog/src/windows
${LiveMediaExt_SOURCE_DIR}/../externals/glog/src/windows/glog
)

SET(live555Includes
${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/live/BasicUsageEnvironment/include
${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/live/groupsock/include
${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/live/liveMedia/include
${LiveMediaExt_SOURCE_DIR}/../externals/live555-CMake/live/UsageEnvironment/include
)

IF(WIN32)
SET(LiveMediaExtIncludes
${LiveMediaExt_SOURCE_DIR}
${LiveMediaExt_SOURCE_DIR}/lib
${LiveMediaExt_SOURCE_DIR}/libs
${LiveMediaExt_SOURCE_DIR}/../include/
${live555Includes}
${LiveMediaExt_SOURCE_DIR}/../externals/glog/src/windows
${LiveMediaExt_SOURCE_DIR}/../externals/cpp-util/include
$ENV{BOOST_INCLUDE_DIR}
)
ELSEIF(UNIX)
# Include directories
SET(LiveMediaExtIncludes
${LiveMediaExt_SOURCE_DIR}
${LiveMediaExt_SOURCE_DIR}/lib
${LiveMediaExt_SOURCE_DIR}/libs
${LiveMediaExt_SOURCE_DIR}/../include
${live555Includes}
${LiveMediaExt_SOURCE_DIR}/../externals/glog/src/windows
${LiveMediaExt_SOURCE_DIR}/../externals/cpp-util/include
/usr/local/include
)
ENDIF(WIN32)

message("LiveMediaExtIncludes directories:" ${LiveMediaExtIncludes})

# lib directories
IF(WIN32)
SET(LiveMediaExtLink
${LiveMediaExt_SOURCE_DIR}/../lib
$ENV{BOOST_LIB_DIR}
)
ELSEIF(UNIX)
SET(LiveMediaExtLink
${LiveMediaExt_SOURCE_DIR}/../lib
${BOOST_LIB_DIR}
/usr/local/lib
/usr/lib
)
ENDIF(WIN32)

message("LiveMediaExtLink directories:" ${LiveMediaExtLink})

# libs
IF(WIN32)
# Do windows specific includes
SET(LiveMediaExtLibs
LiveMediaExt
libglog
)
ELSEIF(APPLE)
# Do apple specific includes
SET(LiveMediaExtLibs
LiveMediaExt
glog
boost_date_time boost_filesystem boost_system boost_thread boost_program_options boost_regex
pthread
c++
)
ELSEIF(UNIX)
# Do linux specific includes
SET(LiveMediaExtLibs
LiveMediaExt
glog
boost_date_time boost_filesystem boost_system boost_thread boost_program_options boost_regex
pthread
)
ENDIF(WIN32) 

SET(LibDir
${LiveMediaExt_SOURCE_DIR}/../lib
$ENV{BOOST_LIB_DIR}
)
message("LibDir directories:" ${LibDir})

# Include directories
INCLUDE_DIRECTORIES(
${LiveMediaExt_SOURCE_DIR}/../include
$ENV{CPP_UTIL_DIR}/include
)

ADD_SUBDIRECTORY( apps)
ADD_SUBDIRECTORY( lib) 
ADD_SUBDIRECTORY( libs) 
# HACK TO BUILD glog CMake project from this CMake project
IF (BUILD_GLOG)
ADD_SUBDIRECTORY (../externals/glog/src ${LiveMediaExt_SOURCE_DIR}/../projects-vs2013)
ENDIF (BUILD_GLOG)
# HACK TO BUILD LIVE555 CMake project from this CMake project
IF (BUILD_LIVE555)
ADD_SUBDIRECTORY (../externals/live555-CMake/live ${LiveMediaExt_SOURCE_DIR}/../projects-vs2013)
ENDIF (BUILD_LIVE555)
