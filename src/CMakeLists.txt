
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

ADD_SUBDIRECTORY(idecore)
ADD_SUBDIRECTORY(microide)

FIND_PACKAGE(Boost COMPONENTS system filesystem program_options REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/idecore/include ${Boost_INCLUDE_DIRS})

ADD_EXECUTABLE(project_test test.cpp)
TARGET_LINK_LIBRARIES(project_test idecore)

