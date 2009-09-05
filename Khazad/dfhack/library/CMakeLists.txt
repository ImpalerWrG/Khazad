SET(PROJECT_SRCS
DataModel.cpp
DfMap.cpp
Extract.cpp
LoadSave.cpp
LoadSaveV1.cpp
#LoadSaveV2.cpp
#LoadSaveV3.cpp
MemInfo.cpp
Process.cpp
ProcessManager.cpp
SimpleMapAPI.cpp
md5/md5.cpp
md5/md5wrapper.cpp
tinyxml/tinystr.cpp
tinyxml/tinystr.h
tinyxml/tinyxml.cpp
tinyxml/tinyxml.h
tinyxml/tinyxmlerror.cpp
tinyxml/tinyxmlparser.cpp
)

IF(UNIX)
  add_definitions(-DLINUX_BUILD)
ELSE(UNIX)
  SET(PROJECT_LIBS psapi)
ENDIF(UNIX)

ADD_LIBRARY(dfhack SHARED ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(dfhack ${PROJECT_LIBS})