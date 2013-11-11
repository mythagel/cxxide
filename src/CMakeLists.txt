
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

ADD_SUBDIRECTORY(idecore)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/idecore)
ADD_EXECUTABLE(project_test test.cpp)
TARGET_LINK_LIBRARIES(project_test idecore)

