

FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)

INCLUDE_DIRECTORIES(../system/ ${CMAKE_SOURCE_DIR}/lib/cmakep ${Boost_INCLUDE_DIRS})

ADD_LIBRARY(cmake_config STATIC cmake.cpp listreader.cpp listwriter.cpp)
TARGET_LINK_LIBRARIES(cmake_config cmakep ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

ADD_LIBRARY(cmake STATIC cmake_project.cpp)
TARGET_LINK_LIBRARIES(cmake cmake_config system ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
