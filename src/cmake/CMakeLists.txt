
INCLUDE_DIRECTORIES(../system/)
ADD_LIBRARY(cmake STATIC cmake.cpp listparser.cpp)
TARGET_LINK_LIBRARIES(cmake system)

