cmake_minimum_required (VERSION 2.6) 
project (Streetview)

#Prefer static libs
if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
else(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
endif(WIN32)

file(GLOB Sources *.cpp)

# Add icon file
if(APPLE)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/../streetview-explorer.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(Sources ${Sources} ${CMAKE_CURRENT_SOURCE_DIR}/../streetview-explorer.icns)
endif(APPLE)

# Define executable
add_executable(streetview MACOSX_BUNDLE ${Sources})
set_target_properties(streetview PROPERTIES OUTPUT_NAME "Streetview Explorer")

# Include dependencies
find_package(wxWidgets REQUIRED html adv core base net aui xrc qa richtext gl)
find_package(jpeg REQUIRED)
find_package(curl REQUIRED)
find_package(zlib REQUIRED)

include(${wxWidgets_USE_FILE})
include_directories(${JPEG_INCLUDE_DIR} ${CURL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
target_link_libraries(streetview ${JPEG_LIBRARY} ${CURL_LIBRARY} ${ZLIB_LIBRARY} ${wxWidgets_LIBRARIES})

# Set bundle properties
if(APPLE)
    set_target_properties(streetview PROPERTIES
        MACOSX_BUNDLE_ICON_FILE streetview-explorer.icns
        MACOSX_BUNDLE_BUNDLE_NAME "Streetview Explorer"
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.0"
        MACOSX_BUNDLE_INFO_STRING "Streetview Explorer 1.0"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.opensource.streetview-explorer"
        MACOSX_BUNDLE_COPYRIGHT "Paul Wagener")
endif(APPLE)