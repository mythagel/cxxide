
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PANGOCAIRO REQUIRED pangocairo)

INCLUDE_DIRECTORIES(${PANGOCAIRO_INCLUDE_DIRS})

ADD_LIBRARY(pango++ STATIC context.cpp layout.cpp)
TARGET_LINK_LIBRARIES(pango++ ${PANGOCAIRO_LDFLAGS})

