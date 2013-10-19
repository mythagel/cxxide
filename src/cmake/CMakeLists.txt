
INCLUDE_DIRECTORIES(../system/)
ADD_LIBRARY(cmake STATIC cmake.cpp listparser.cpp listreader.cpp listwriter.cpp)
TARGET_LINK_LIBRARIES(cmake system)

