cmake_minimum_required(VERSION 3.2)
project(OpenCV_Blueprints_03_OpenCV_3)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIR})

# build libflandmark shared and static libraries
add_subdirectory(libflandmark)
include_directories("${PROJECT_SOURCE_DIR}/libflandmark")

FILE(COPY "haarcascade_frontalface_alt.xml" DESTINATION ${CMAKE_BINARY_DIR})
FILE(COPY "flandmark_model.dat" DESTINATION ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")

set(FACIAL_COMPONENTS_FILES facial_components.cpp dirent.h)
add_executable(facial_components ${FACIAL_COMPONENTS_FILES})
target_link_libraries( facial_components flandmark_static ${OpenCV_LIBS} )

set(FEATURE_EXTRACTION_FILES feature_extraction.cpp)
add_executable(feature_extraction ${FEATURE_EXTRACTION_FILES})
target_link_libraries( feature_extraction ${OpenCV_LIBS} )

set(TRAIN_FILES train.cpp)
add_executable(train ${TRAIN_FILES})
target_link_libraries( train ${OpenCV_LIBS} )