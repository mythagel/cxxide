
FIND_PACKAGE(Boost REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib/template)

ADD_SUBDIRECTORY(system)
ADD_SUBDIRECTORY(git)
ADD_SUBDIRECTORY(cmake)
ADD_SUBDIRECTORY(source)

ADD_LIBRARY(idecore STATIC idecore.cpp)
TARGET_LINK_LIBRARIES(idecore git cmake template)

ADD_EXECUTABLE(project_test test.cpp)
TARGET_LINK_LIBRARIES(project_test idecore)

