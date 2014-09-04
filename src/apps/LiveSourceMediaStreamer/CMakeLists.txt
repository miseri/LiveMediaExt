# CMakeLists.txt in LiveSourceMediaStreamer

SET(LSMS_HDRS
stdafx.h
VersionInfo.h
)

SET(LSMS_SRCS
main.cpp
)

INCLUDE_DIRECTORIES(
${LiveMediaExtIncludes}
)

LINK_DIRECTORIES(
${LiveMediaExtLink}
)

ADD_EXECUTABLE(LiveSourceMediaStreamer ${LSMS_HDRS} ${LSMS_SRCS})

TARGET_LINK_LIBRARIES (
LiveSourceMediaStreamer
${LiveMediaExtLibs}
SimpleRateAdaptation
BasicUsageEnvironment
groupsock
liveMedia
UsageEnvironment
)

install(TARGETS LiveSourceMediaStreamer
            RUNTIME DESTINATION ${LiveMediaExt_BIN}
            LIBRARY DESTINATION ${LiveMediaExt_BIN}
            ARCHIVE DESTINATION ${LiveMediaExt_SOURCE_DIR}/../lib)

