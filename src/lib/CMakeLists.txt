# CMakeLists.txt in lib dir

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledBinary}\" /FI\"${PrecompiledBinary}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
  message("Clang detected: setting warning level")
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-unused-function")
#  ADD_DEFINITIONS (-Werror )
#  ADD_DEFINITIONS (-Wall )
#  ADD_DEFINITIONS (/W4 )
#  set_property( GLOBAL APPEND_STRING PROPERTY COMPILE_FLAGS -Werror )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

SET(LMX_SRCS 
LiveMediaExtPch.cpp
LiveDeviceSource.cpp
)
SET(LMX_HEADERS
LiveMediaExtPch.h
../../include/LiveMediaExt/LiveDeviceSource.h
)
SET(MED_SRCS
RtspService.cpp
VirtualMediaSource.cpp
)
SET(MED_HEADERS
../../include/Media/MediaChannel.h
../../include/Media/MediaSample.h
../../include/Media/PacketManagerMediaChannel.h
../../include/Media/RtspService.h
../../include/Media/VirtualMediaSource.h
)
SET(CPP_UTIL_HEADERS
../../externals/cpp-util/include/cpputil/Buffer.h
../../externals/cpp-util/include/cpputil/Clock.h
../../externals/cpp-util/include/cpputil/ConsoleApplicationUtil.h
../../externals/cpp-util/include/cpputil/Conversion.h
../../externals/cpp-util/include/cpputil/ExceptionBase.h
../../externals/cpp-util/include/cpputil/FileUtil.h
../../externals/cpp-util/include/cpputil/IBitStream.h
../../externals/cpp-util/include/cpputil/MakeService.h
../../externals/cpp-util/include/cpputil/OBitStream.h
../../externals/cpp-util/include/cpputil/RandomUtil.h
../../externals/cpp-util/include/cpputil/RunningAverageQueue.h
../../externals/cpp-util/include/cpputil/ServiceController.h
../../externals/cpp-util/include/cpputil/ServiceManager.h
../../externals/cpp-util/include/cpputil/ServiceThread.h
../../externals/cpp-util/include/cpputil/StringTokenizer.h
../../externals/cpp-util/include/cpputil/Utility.h
)

SET(ALL_SRCS
${LMX_SRCS} 
${MED_SRCS} 
)

SOURCE_GROUP("Source Files\\LiveMediaExt" FILES ${LMX_SRCS})
SOURCE_GROUP("Source Files\\Media" FILES ${MED_SRCS})

SOURCE_GROUP("Header Files\\cpp-util" FILES ${CPP_UTIL_HEADERS})
SOURCE_GROUP("Header Files\\LiveMediaExt" FILES ${LMX_HEADERS})
SOURCE_GROUP("Header Files\\Media" FILES ${MED_HEADERS})

SET(ALL_HEADERS 
${CPP_UTIL_HEADERS}
${LMX_HEADERS}
${MED_HEADERS}
)

INCLUDE_DIRECTORIES(
${LiveMediaExtIncludes}
)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
# 64bit
  SET(CMAKE_CXX_FLAGS "-fPIC")
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
# 32 bit
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

ADD_LIBRARY( LiveMediaExt STATIC ${ALL_SRCS} ${ALL_HEADERS})

# adding precompiled header support on windows
if (MSVC)
   set_target_properties(LiveMediaExt PROPERTIES COMPILE_FLAGS "/YuLiveMediaExtPch.h")
   set_source_files_properties(LiveMediaExtPch.cpp PROPERTIES COMPILE_FLAGS "/YcLiveMediaExtPch.h")
endif(MSVC)
