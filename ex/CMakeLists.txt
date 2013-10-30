
ADD_CUSTOM_TARGET(experiments)

ADD_EXECUTABLE(xcbpp EXCLUDE_FROM_ALL xcb.cpp)
TARGET_LINK_LIBRARIES(xcbpp xcb)
ADD_DEPENDENCIES(experiments xcbpp)


ADD_EXECUTABLE(libclang_basic EXCLUDE_FROM_ALL libclang_basic.cpp)
TARGET_LINK_LIBRARIES(libclang_basic clang)
ADD_DEPENDENCIES(experiments libclang_basic)

FIND_PACKAGE(X11 REQUIRED)
INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR} ${X11_Xt_INCLUDE_PATH} /usr/include/X11 /usr/include/freetype2)
ADD_EXECUTABLE(xft_basic EXCLUDE_FROM_ALL xft_basic.cpp)
TARGET_LINK_LIBRARIES(xft_basic ${X11_LIBRARIES} ${X11_Xt_LIB} Xft)
ADD_DEPENDENCIES(experiments xft_basic)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(CAIRO REQUIRED cairo)
PKG_CHECK_MODULES(PANGOCAIRO REQUIRED pangocairo)

INCLUDE_DIRECTORIES(${CAIRO_INCLUDE_DIRS} ${PANGOCAIRO_INCLUDE_DIRS} 
${CMAKE_SOURCE_DIR}/lib/x11++/include 
${CMAKE_SOURCE_DIR}/lib/cairo++/include
${CMAKE_SOURCE_DIR}/lib/pango++/include
)

ADD_EXECUTABLE(ui_test EXCLUDE_FROM_ALL ui_test.cpp)
TARGET_LINK_LIBRARIES(ui_test cairo++ pango++)
ADD_DEPENDENCIES(experiments ui_test)


ADD_EXECUTABLE(hash_image EXCLUDE_FROM_ALL hash_image.cpp)
ADD_DEPENDENCIES(experiments hash_image)


