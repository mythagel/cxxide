
INCLUDE_DIRECTORIES(../system/ ${CMAKE_SOURCE_DIR}/lib/cmakep)
ADD_LIBRARY(cmake STATIC cmake.cpp listreader.cpp listwriter.cpp)
TARGET_LINK_LIBRARIES(cmake system cmakep)

