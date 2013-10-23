
ADD_CUSTOM_TARGET(experiments)

ADD_EXECUTABLE(libclang_basic EXCLUDE_FROM_ALL libclang_basic.cpp)
TARGET_LINK_LIBRARIES(libclang_basic clang)
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

ADD_EXECUTABLE(hash_image EXCLUDE_FROM_ALL hash_image.cpp)
ADD_DEPENDENCIES(experiments hash_image)


