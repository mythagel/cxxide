
INCLUDE_DIRECTORIES(../system/ ${CMAKE_SOURCE_DIR}/lib/cmakep)

ADD_LIBRARY(cmake_config STATIC cmake.cpp listreader.cpp listwriter.cpp)
TARGET_LINK_LIBRARIES(cmake_config cmakep)

ADD_LIBRARY(cmake STATIC cmake_project.cpp)
TARGET_LINK_LIBRARIES(cmake cmake_config system)
