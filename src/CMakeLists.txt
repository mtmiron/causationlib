cmake_minimum_required(VERSION 2.8)
project( CAUSATIONLIB )

find_package( OpenCV REQUIRED )

set( CMAKE_BUILD_TYPE DEBUG )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O3" )
set( NETPEEK_LIBS "" )

option( BUILD_TESTING OFF )
option( BUILD_WITH_TIMEQUEUE "Use the TimeQueue" ON )
option( BUILD_SHARED_LIBS "Build shared libraries" OFF )
option( BUILD_WITH_OPENNI "Use OpenNI functions & Kinect video channels" OFF )
option( BUILD_WITH_MULTITHREADING "Use multiple threads of execution" OFF )

if ( BUILD_WITH_TIMEQUEUE )
	MESSAGE( STATUS "TimeQueue code enabled" )
	add_definitions( -DBUILD_WITH_TIMEQUEUE )
endif()

if (BUILD_SHARED_LIBS)
	MESSAGE( STATUS "Building shared libraries" )
	add_definitions( -DBUILD_SHARED_LIBS )
endif()

if (BUILD_WITH_OPENNI)
	MESSAGE( STATUS "Building with OpenNI (required if this option is set)" )
	add_definitions( -DBUILD_WITH_OPENNI )
endif()

if (BUILD_WITH_MULTITHREADING)
	MESSAGE( STATUS "Using multiple threads" )
	add_definitions( -DBUILD_WITH_MULTITHREADING )
	set( NETPEEK_LIBS "pthread" )
endif()

add_library( temporalnn temporalnn.cc )
add_library( nnvisualizer nnvisualizer.cc )
add_library( tortoise tortoise.cc )
add_library( randomnn randomnn.cc )

add_executable( causation causation.cc )
add_executable( netpeek netpeek.cc )

target_link_libraries( causation ${OpenCV_LIBS} tortoise )
target_link_libraries( netpeek ${NETPEEK_LIBS} ${OpenCV_LIBS} temporalnn nnvisualizer tortoise randomnn )
