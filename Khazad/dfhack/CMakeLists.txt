PROJECT (dfhack) 
cmake_minimum_required(VERSION 2.6)

SUBDIRS (tinyxml)

# disable warning, autosearch for libraries
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET(PROJECT_SRCS
DataModel.cpp
#DfMap.cpp
#Extract.cpp
#LoadSave.cpp
#LoadSaveV1.cpp
#LoadSaveV2.cpp
#LoadSaveV3.cpp
MemInfo.cpp
Process.cpp
ProcessManager.cpp
SimpleMapAPI.cpp
md5/md5.cpp
md5/md5wrapper.cpp
)

IF(UNIX)
  add_definitions(-DLINUX_BUILD)
  SET(PROJECT_LIBS mhash tinyxml)
  SET(PROJECT_SRCS
   ${PROJECT_SRCS}
  md5.cpp
  )
ELSE(UNIX)
  SET(PROJECT_LIBS tinyxml psapi)
ENDIF(UNIX)


ADD_LIBRARY(dfhack SHARED ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(dfhack ${PROJECT_LIBS})


