SET(TINYXML_SRCS
  tinystr.cpp
  tinystr.h
  tinyxml.cpp
  tinyxml.h
  tinyxmlerror.cpp
  tinyxmlparser.cpp
)

ADD_LIBRARY(tinyxml STATIC ${TINYXML_SRCS}) 
