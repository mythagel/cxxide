
ADD_CUSTOM_TARGET(experiments)

ADD_EXECUTABLE(xcb_basic EXCLUDE_FROM_ALL xcb_basic.cpp)
TARGET_LINK_LIBRARIES(xcb_basic xcb)
ADD_DEPENDENCIES(experiments xcb_basic)

INCLUDE_DIRECTORIES("/usr/include/llvm-3.2/")
ADD_EXECUTABLE(clang_basic EXCLUDE_FROM_ALL clang_basic.cpp)
TARGET_LINK_LIBRARIES(clang_basic xcb)
ADD_DEPENDENCIES(experiments clang_basic)

