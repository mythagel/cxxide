
ADD_CUSTOM_TARGET(experiments)

ADD_EXECUTABLE(xcb_basic EXCLUDE_FROM_ALL xcb_basic.cpp)
TARGET_LINK_LIBRARIES(xcb_basic xcb)
ADD_DEPENDENCIES(experiments xcb_basic)

#INCLUDE_DIRECTORIES("/home/nicholas/dev/build/llvm/include" "/home/nicholas/dev/llvm/tools/clang/include/" "/home/nicholas/dev/llvm/include/"
#"/home/nicholas/dev/build/llvm/tools/clang/include/")
#ADD_EXECUTABLE(clang_basic EXCLUDE_FROM_ALL clang_basic.cpp)
#TARGET_LINK_LIBRARIES(clang_basic xcb)
#ADD_DEPENDENCIES(experiments clang_basic)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/llvm/tools/clang/include/)
ADD_EXECUTABLE(libclang_basic EXCLUDE_FROM_ALL libclang_basic.cpp)
TARGET_LINK_LIBRARIES(libclang_basic libclang)
ADD_DEPENDENCIES(experiments libclang_basic)


FIND_PACKAGE(X11 REQUIRED)

INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR} ${X11_Xt_INCLUDE_PATH})
ADD_EXECUTABLE(xaw_basic EXCLUDE_FROM_ALL xaw_basic.cpp)
TARGET_LINK_LIBRARIES(xaw_basic ${X11_LIBRARIES} ${X11_Xt_LIB} Xaw Xmu Xt SM ICE Xext X11)
ADD_DEPENDENCIES(experiments xaw_basic)

ADD_EXECUTABLE(frame EXCLUDE_FROM_ALL frame.cpp)
TARGET_LINK_LIBRARIES(frame ${X11_LIBRARIES} ${X11_Xt_LIB} Xaw Xmu Xt SM ICE Xext X11)
ADD_DEPENDENCIES(experiments frame)

INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR} ${X11_Xt_INCLUDE_PATH} /usr/include/X11 /usr/include/freetype2)
ADD_EXECUTABLE(xft_basic EXCLUDE_FROM_ALL xft_basic.cpp)
TARGET_LINK_LIBRARIES(xft_basic ${X11_LIBRARIES} ${X11_Xt_LIB} Xft Xaw Xmu Xt SM ICE Xext X11)
ADD_DEPENDENCIES(experiments xft_basic)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_BINARY_DIR}/deps/llvm/share/llvm/cmake/")
include(LLVMConfig)

include_directories( ${LLVM_INCLUDE_DIRS} 
${PROJECT_SOURCE_DIR}/deps/llvm/include/ 
${CMAKE_BINARY_DIR}/deps/llvm/include/
${PROJECT_SOURCE_DIR}/deps/llvm/tools/clang/include/
${CMAKE_BINARY_DIR}/deps/llvm/tools/clang/include/
)
link_directories( ${LLVM_LIBRARY_DIRS} )
add_definitions( ${LLVM_DEFINITIONS} )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -g3")
ADD_EXECUTABLE(Indexer EXCLUDE_FROM_ALL Indexer.cpp)
llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES core)
target_link_libraries(Indexer ${REQ_LLVM_LIBRARIES} clangBasic clangFrontend clangLex clangAST clangSema clangParse)
ADD_DEPENDENCIES(experiments Indexer)

ADD_EXECUTABLE(ci_token_lexer EXCLUDE_FROM_ALL ci_token_lexer.cpp)
llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES core)
target_link_libraries(ci_token_lexer ${REQ_LLVM_LIBRARIES} clangBasic clangFrontend clangLex clangAST clangSema clangParse)
ADD_DEPENDENCIES(experiments ci_token_lexer)

include_directories( ${PROJECT_SOURCE_DIR}/deps/libgit2/include/ )
ADD_EXECUTABLE(git_basic EXCLUDE_FROM_ALL git_basic.cpp)
TARGET_LINK_LIBRARIES(git_basic git2)
ADD_DEPENDENCIES(experiments git_basic)

ADD_EXECUTABLE(hash_image EXCLUDE_FROM_ALL hash_image.cpp)
ADD_DEPENDENCIES(experiments hash_image)


