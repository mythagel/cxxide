
FIND_PACKAGE(Boost COMPONENTS system filesystem program_options REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/idecore/include ${Boost_INCLUDE_DIRS})
ADD_EXECUTABLE(microide microide.cpp linenoise.c)
TARGET_LINK_LIBRARIES(microide idecore ${Boost_LIBRARIES} )

