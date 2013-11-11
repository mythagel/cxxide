

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

ADD_SUBDIRECTORY(src)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/idecore/include)
ADD_LIBRARY(idecore STATIC idecore.cpp)
TARGET_LINK_LIBRARIES(idecore git cmake template)

