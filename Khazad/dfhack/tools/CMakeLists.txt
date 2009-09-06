# map export benchmark

IF(UNIX)
add_definitions(-DLINUX_BUILD)
ELSE(UNIX)
SET(PROJECT_LIBS psapi)
ENDIF(UNIX)

ADD_EXECUTABLE(expbench expbench.cpp)
TARGET_LINK_LIBRARIES(expbench dfhack)