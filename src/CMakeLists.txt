cmake_minimum_required (VERSION 2.6) 
project (Streetview)

#Prefer static libs
if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
else(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
endif(WIN32)

file(GLOB Sources *.cpp)

add_executable(streetview ${Sources})

find_package(wxWidgets REQUIRED html adv core base net aui xrc qa richtext gl)
find_package(jpeg REQUIRED)
find_package(curl REQUIRED)
find_package(zlib REQUIRED)

include(${wxWidgets_USE_FILE})
include_directories(${JPEG_INCLUDE_DIR} ${CURL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
target_link_libraries(streetview ${JPEG_LIBRARY} ${CURL_LIBRARY} ${ZLIB_LIBRARY} ${wxWidgets_LIBRARIES})