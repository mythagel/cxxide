
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

# FIND_PACKAGE(Cairo REQUIRED)

ADD_LIBRARY(cairo++ STATIC cairowindow.cpp colour.cpp drawing.cpp)
TARGET_LINK_LIBRARIES(cairo++ ${X11_LIBRARIES} x11++ cairo)

