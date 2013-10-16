CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(example)

#Project wide (top level CMakeLists.txt only)
##<< Referenced Packages >>##
FIND_PACKAGE(Foo)
FIND_PACKAGE(Bar)
FIND_PACKAGE(Baz)
##<< Referenced Packages >>##

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(DeclTarget) # cmake/Modules/DeclTarget.cmake
FUNCTION(DECLARE_TARGET target_name)
    STRING(TOUPPER "${target_name}" TARGET)

    IF("${TARGET}_TYPE" STREQUAL "EXECUTABLE")
        ADD_EXECUTABLE("${target_name}" ${TARGET}_SOURCES)
        
        IF(DEFINED "${TARGET}_VERSION")
            SET_PROPERTY(TARGET foo PROPERTY VERSION ${TARGET}_VERSION)
        ENDIF()
    
    ELSEIF("${TARGET}_TYPE" STREQUAL "SHARED_LIBRARY")
        ADD_LIBRARY("${target_name}" SHARED ${TARGET}_SOURCES)
        
        IF(DEFINED "${TARGET}_VERSION")
            STRING(REPLACE "." ";" VERSION "${TARGET}_VERSION")
            list(GET ${VERSION} 0 VERSION_MAJOR)
            SET_PROPERTY(TARGET foo PROPERTY SOVERSION ${VERSION_MAJOR})
            SET_PROPERTY(TARGET foo PROPERTY VERSION ${TARGET}_VERSION)
        ENDIF()
    
    ELSEIF("${TARGET}_TYPE" STREQUAL "STATIC_LIBRARY")
        ADD_LIBRARY("${target_name}" STATIC ${TARGET}_SOURCES)
        
        IF(DEFINED "${TARGET}_VERSION")
            SET_PROPERTY(TARGET foo PROPERTY VERSION ${TARGET}_VERSION)
        ENDIF()
    
    ELSE()
        MESSAGE(FATAL_ERROR "Unknown target type '${TARGET}_TYPE' in target '${target_name}'")
    ENDIF()
    
    IF(DEFINED "${TARGET}_LABEL")
        SET_PROPERTY(TARGET ${target_name} PROPERTY PROJECT_LABEL "${TARGET}_LABEL")
    ENDIF()
    
    IF(DEFINED "${TARGET}_DEFINES")
        SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY COMPILE_DEFINITIONS ${TARGET}_DEFINES)
    ENDIF()

    IF(DEFINED "${TARGET}_INCLUDES")
        SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY INCLUDE_DIRECTORIES ${TARGET}_INCLUDES)
    ENDIF()
    
    IF(DEFINED "${TARGET}_COMPILEFLAGS")
        SET_PROPERTY(TARGET ${target_name} APPEND_STRING PROPERTY COMPILE_FLAGS "${TARGET}_COMPILEFLAGS")
    ENDIF()

    IF(DEFINED "${TARGET}_LINKFLAGS")
        SET_PROPERTY(TARGET ${target_name} APPEND_STRING PROPERTY LINK_FLAGS "${TARGET}_LINKFLAGS")
    ENDIF()
    
    FOREACH(library ${TARGET}_LIBS)
        FIND_LIBRARY("${library}" ${library})
        LIST(APPEND "${TARGET}_RESOLVED_LIBS" ${${library}})
    ENDFOREACH()
    
    FOREACH(package ${TARGET}_PACKAGES)
        STRING(TOUPPER "${package}" upper_package)
        
        IF("${package}_FOUND")
            SET(PKG ${package})
        ELSEIF("${upper_package}_FOUND")
            SET(PKG ${upper_package})
        ELSE()
            # Not actually an accurate message - more accurate is that it is not recognised.
            MESSAGE(FATAL_ERROR "Required package '${package}' in target '${target_name} NOTFOUND'")
        ENDIF()
        
        IF(DEFINED "${PKG}_DEFINITIONS")
            SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY COMPILE_DEFINITIONS ${PKG}_DEFINITIONS)
        ENDIF()
        
        IF(DEFINED "${PKG}_INCLUDE_DIRS")
            SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY INCLUDE_DIRECTORIES ${PKG}_INCLUDE_DIRS)
        ELSEIF(DEFINED "${PKG}_INCLUDE_DIR")
            SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY INCLUDE_DIRECTORIES ${PKG}_INCLUDE_DIR)
        ENDIF()
        
        IF(DEFINED "${PKG}_LIBRARIES")
            LIST(APPEND "${TARGET}_RESOLVED_LIBS" ${PKG}_LIBRARIES)
        ELSEIF(DEFINED "${PKG}_LIBRARY")
            LIST(APPEND "${TARGET}_RESOLVED_LIBS" ${PKG}_LIBRARY)
        ENDIF()
        
    ENDFOREACH()
    
    IF(DEFINED "RESOLVED_LIBS")
        TARGET_LINK_LIBRARIES(${target_name} ${TARGET}_RESOLVED_LIBS)
    ENDIF()
    
    IF(DEFINED "${TARGET}_DEPENDS")
        ADD_DEPENDENCIES(${target_name} ${TARGET}_DEPENDS)
    ENDIF()
ENDFUNCTION()

##<< Folder Properties >>##
ADD_DEFINITIONS(-DFOO -DBAR ...)
INCLUDE_DIRECTORIES(dir1 dir2 ...)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11")
CONFIGURE_FILE(<input1> <output1>)
CONFIGURE_FILE(<input2> <output2>)
ADD_SUBDIRECTORY(blah1)
ADD_SUBDIRECTORY(blah2)
##<< Folder Properties >>##

##<< File Properties >>##
SET_PROPERTY(SOURCE a.cpp APPEND PROPERTY COMPILE_DEFINITIONS -DBLAH)
SET_PROPERTY(SOURCE a.cpp APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -Wextra")
##<< File Properties >>##

##<< Target 'foo' >>##
set(FOO_LABEL "Foo Library")
set(FOO_TYPE EXECUTABLE SHARED_LIBRARY STATIC_LIBRARY)
set(FOO_VERSION 0.0.0)
set(FOO_SOURCES 
a.cpp 
b.cpp 
c.cpp)
set(FOO_DEFINES -DBLAH -DBLEH)
set(FOO_INCLUDES ${PROJECT_SOURCE_DIR}/include /usr/local/include)
set(FOO_COMPILEFLAGS -Wall -Wextra -std=c++11)
set(FOO_LIBS m)
set(FOO_LINKFLAGS -fPIC)
set(FOO_PACKAGES Bar Baz)
set(FOO_DEPENDS bar)
DECLARE_TARGET(foo)
##<< Target 'foo' >>##


