# The name of our project is "HELLO". CMakeLists files in this project can 
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and 
# to the root binary directory of the project as ${HELLO_BINARY_DIR}. 
cmake_minimum_required (VERSION 2.6) 
project (Streetview)

FILE(GLOB Sources *.cpp)

ADD_EXECUTABLE(streetview ${Sources})

FIND_PACKAGE(wxWidgets REQUIRED html adv core base net aui xrc qa richtext gl)
find_package(jpeg REQUIRED)
find_package(curl REQUIRED)
find_package(zlib REQUIRED)

include(${wxWidgets_USE_FILE})
include_directories(${JPEG_INCLUDE_DIR} ${CURL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
target_link_libraries(streetview ${wxWidgets_LIBRARIES} ${JPEG_LIBRARY} ${CURL_LIBRARY} ${ZLIB_LIBRARY})