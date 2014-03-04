
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

ADD_LIBRARY(ex STATIC ex.cpp buffer.cpp cursor.cpp)
#TARGET_LINK_LIBRARIES(ex clang)

