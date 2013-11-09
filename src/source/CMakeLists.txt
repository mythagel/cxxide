
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib/template)
ADD_LIBRARY(source STATIC source.cpp license.cpp)
TARGET_LINK_LIBRARIES(source template)

