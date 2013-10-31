
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

ADD_LIBRARY(clang++ STATIC clang++.cpp)
TARGET_LINK_LIBRARIES(clang++ clang)

