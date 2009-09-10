# map export benchmark

# this is required to ensure we use the right configuration of header files
IF(UNIX)
add_definitions(-DLINUX_BUILD)
ENDIF(UNIX)

# a benchmark program
ADD_EXECUTABLE(expbench expbench.cpp)
TARGET_LINK_LIBRARIES(expbench dfhack)

# a reveal clone
ADD_EXECUTABLE(reveal reveal.cpp)
TARGET_LINK_LIBRARIES(reveal dfhack)

# prospector - produces a list of available materials and their quantities
ADD_EXECUTABLE(prospector prospector.cpp)
TARGET_LINK_LIBRARIES(prospector dfhack)