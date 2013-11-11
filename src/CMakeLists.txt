
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

ADD_SUBDIRECTORY(idecore)

FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/idecore/include ${Boost_INCLUDE_DIRS})

ADD_EXECUTABLE(project_test test.cpp)
TARGET_LINK_LIBRARIES(project_test idecore)

ADD_EXECUTABLE(microide microide.cpp)
TARGET_LINK_LIBRARIES(microide idecore ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} )

