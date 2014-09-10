# CMakeLists.txt in SimpleRateAdaptation dir

SET(SRA_SRCS 
SimpleRateAdaptation.cpp
SimpleRateAdaptationFactory.cpp
SimpleRateAdaptationPch.cpp
)
SET(SRA_HEADERS
SimpleRateAdaptationPch.h
SimpleRateAdaptation.h
SimpleRateAdaptationFactory.h
)

INCLUDE_DIRECTORIES(
${LiveMediaExtIncludes}
)

ADD_LIBRARY( SimpleRateAdaptation SHARED ${SRA_SRCS} ${SRA_HEADERS})

TARGET_LINK_LIBRARIES (
SimpleRateAdaptation
${LiveMediaExtLibs}
)

IF(WIN32)
add_custom_command(TARGET SimpleRateAdaptation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIBRARY_OUTPUT_PATH}/$<CONFIGURATION>/SimpleRateAdaptation.dll"
        "${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>/SimpleRateAdaptation.dll")
ENDIF(WIN32)
