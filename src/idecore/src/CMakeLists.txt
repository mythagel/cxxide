
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/lib/template
    ${CMAKE_SOURCE_DIR}/src/idecore/include
)

ADD_SUBDIRECTORY(system)
ADD_SUBDIRECTORY(git)
ADD_SUBDIRECTORY(cmake)
ADD_SUBDIRECTORY(source)

ADD_LIBRARY(idecore STATIC idecore.cpp)
TARGET_LINK_LIBRARIES(idecore git cmake template ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

