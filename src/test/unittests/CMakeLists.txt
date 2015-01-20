# CMakeLists.txt in unittest dir

# source files for unittest
SET(UNIT_TEST_SRCS 
main.cpp
)

INCLUDE_DIRECTORIES(
${LiveMediaExtIncludes}
$ENV{BOOST_INCLUDE_DIR}
)

LINK_DIRECTORIES(
${LiveMediaExtLink}
)

ADD_EXECUTABLE(unittest ${UNIT_TEST_SRCS})

TARGET_LINK_LIBRARIES (
unittest
${LiveMediaExtLibs}
)
