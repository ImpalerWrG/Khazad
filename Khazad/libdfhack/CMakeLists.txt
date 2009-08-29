PROJECT (dfhack) 

SUBDIRS (tinyxml)

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
ADD_LIBRARY(dfhack-static STATIC ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(dfhack-static ${PROJECT_LIBS})
TARGET_LINK_LIBRARIES(dfhack ${PROJECT_LIBS})

SET_TARGET_PROPERTIES(dfhack-static PROPERTIES OUTPUT_NAME "dfhack")

