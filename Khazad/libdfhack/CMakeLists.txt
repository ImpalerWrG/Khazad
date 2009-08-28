PROJECT (dfhack) 

SUBDIRS (tinyxml)

SET(PROJECT_SRCS
DataModel.cpp
DfMap.cpp
Extract.cpp
LoadSave.cpp
LoadSaveV1.cpp
LoadSaveV2.cpp
LoadSaveV3.cpp
MemInfo.cpp
Process.cpp
ProcessManager.cpp
SimpleMapAPI.cpp
md5.cpp
)

SET(PROJECT_LIBS mhash z tinyxml)

IF(UNIX)
  add_definitions(-DLINUX_BUILD)
ENDIF(UNIX)



ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
