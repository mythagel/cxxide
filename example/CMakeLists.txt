CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(example)

##<< Managed Configuration >>##

#Project wide (top level CMakeLists.txt only)
##<< Referenced Packages >>##
FIND_PACKAGE(Foo)
FIND_PACKAGE(Bar)
FIND_PACKAGE(Baz)
##<< Referenced Packages >>##

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

##<< Directory Properties >>##
ADD_DEFINITIONS(-DFOO -DBAR ...)
INCLUDE_DIRECTORIES(dir1 dir2 ...)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11")
CONFIGURE_FILE(<input1> <output1>)
CONFIGURE_FILE(<input2> <output2>)
ADD_SUBDIRECTORY(blah1)
ADD_SUBDIRECTORY(blah2)
##<< Directory Properties >>##

##<< File Properties >>##
SET_PROPERTY(SOURCE a.cpp APPEND PROPERTY COMPILE_DEFINITIONS -DBLAH)
SET_PROPERTY(SOURCE a.cpp APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -Wextra")
##<< File Properties >>##

INCLUDE( CMakeParseArguments )
FUNCTION( SET_TARGET_LIBRARIES )
    CMAKE_PARSE_ARGUMENTS( arg "" "TARGET" "LIBS" ${ARGV})
    FOREACH(library IN LISTS arg_LIBS)
        FIND_LIBRARY(LIB ${library})
        LIST(APPEND LIBS ${LIB})
    ENDFOREACH()
    TARGET_LINK_LIBRARIES(${TARGET} ${LIBS})
ENDFUNCTION()

FUNCTION( SET_TARGET_PACKAGES )
    CMAKE_PARSE_ARGUMENTS( arg "" "TARGET" "PACKAGES" ${ARGV})
    FOREACH(package IN LISTS arg_PACKAGES)
        STRING(TOUPPER "${package}" upper_package)
        
        IF("${package}_FOUND")
            SET(PKG ${package})
        ELSEIF("${upper_package}_FOUND")
            SET(PKG ${upper_package})
        ELSE()
            # Not actually an accurate message - more accurate is that it is not recognised.
            MESSAGE(FATAL_ERROR "Required package '${package}' in target '${TARGET} NOTFOUND'")
        ENDIF()
        
        IF(DEFINED "${PKG}_DEFINITIONS")
            SET_PROPERTY(TARGET ${TARGET} APPEND PROPERTY COMPILE_DEFINITIONS ${PKG}_DEFINITIONS)
        ENDIF()
        
        IF(DEFINED "${PKG}_INCLUDE_DIRS")
            SET_PROPERTY(TARGET ${TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES ${PKG}_INCLUDE_DIRS)
        ELSEIF(DEFINED "${PKG}_INCLUDE_DIR")
            SET_PROPERTY(TARGET ${TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES ${PKG}_INCLUDE_DIR)
        ENDIF()
        
        IF(DEFINED "${PKG}_LIBRARIES")
            TARGET_LINK_LIBRARIES(${TARGET} ${PKG}_LIBRARIES)
        ELSEIF(DEFINED "${PKG}_LIBRARY")
            TARGET_LINK_LIBRARIES(${TARGET} ${PKG}_LIBRARY)
        ENDIF()
    ENDFOREACH()
ENDFUNCTION()

##<< Target Properties >>##
SET( FOO_SOURCES 
    a.cpp 
    b.cpp 
    c.cpp )
ADD_EXECUTABLE( foo ${FOO_SOURCES} )
SET_TARGET_PACKAGES( TARGET foo PACKAGES Bar Baz )
SET_PROPERTY( TARGET foo APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -Wextra -std=c++11" )
SET_PROPERTY( TARGET foo APPEND PROPERTY COMPILE_DEFINITIONS -DBLAH -DBLEH )
SET_PROPERTY( TARGET foo APPEND PROPERTY INCLUDE_DIRECTORIES 
    ${PROJECT_SOURCE_DIR}/include 
    /usr/local/include )
SET_PROPERTY( TARGET foo PROPERTY PROJECT_LABEL "Foo label" )
SET_PROPERTY( TARGET foo PROPERTY VERSION 0.0.0 )
SET_PROPERTY( TARGET foo PROPERTY SOVERSION 0 )
SET_PROPERTY( TARGET foo APPEND_STRING PROPERTY LINK_FLAGS "-fPIC" )
SET_TARGET_LIBRARIES( TARGET foo LIBS m )
ADD_DEPENDENCIES(foo blah)
##<< Target Properties >>##

