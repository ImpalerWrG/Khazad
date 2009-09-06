# map export benchmark

# this is required to ensure we use the right configuration of header files
IF(UNIX)
add_definitions(-DLINUX_BUILD)
ENDIF(UNIX)

ADD_EXECUTABLE(expbench expbench.cpp)
TARGET_LINK_LIBRARIES(expbench dfhack)