
FIND_PACKAGE(Boost REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

ADD_SUBDIRECTORY(system)
ADD_SUBDIRECTORY(git)
ADD_SUBDIRECTORY(cmake)

ADD_LIBRARY(project STATIC project.cpp)
TARGET_LINK_LIBRARIES(project git cmake)

ADD_EXECUTABLE(project_test test.cpp)
TARGET_LINK_LIBRARIES(project_test project)

