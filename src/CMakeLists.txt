# The name of our project is "HELLO". CMakeLists files in this project can 
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and 
# to the root binary directory of the project as ${HELLO_BINARY_DIR}. 
cmake_minimum_required (VERSION 2.6) 
project (Streetview)

FILE(GLOB Sources *.cpp)

ADD_EXECUTABLE(streetview ${Sources})

FIND_PACKAGE(wxWidgets REQUIRED html adv core base net aui xrc qa richtext gl)
INCLUDE(${wxWidgets_USE_FILE})
TARGET_LINK_LIBRARIES(streetview ${wxWidgets_LIBRARIES}) 

#find_package(Jpeg)
include_directories(/opt/local/include)

TARGET_LINK_LIBRARIES(streetview jpeg z curl)
