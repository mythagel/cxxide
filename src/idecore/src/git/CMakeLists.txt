
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)

INCLUDE_DIRECTORIES(../system/ ${Boost_INCLUDE_DIRS})
ADD_LIBRARY(git STATIC git.cpp)
TARGET_LINK_LIBRARIES(git system ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

