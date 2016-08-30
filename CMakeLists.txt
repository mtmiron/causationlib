cmake_minimum_required(VERSION 2.8)
project( CausationLib )
find_package( OpenCV REQUIRED )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O3" )

add_library( temporalnn SHARED temporalnn.cc )
add_library( nnvisualizer SHARED nnvisualizer.cc )
add_library( tortoise SHARED tortoise.cc )

add_definitions( -DWITH_TORTOISELIB )
#target_compile_definitions( temporalnn PRIVATE WITH_TORTOISELIB=1 )

add_executable( causation causation.cc )
add_executable( netpeek netpeek.cc )

target_link_libraries( causation ${OpenCV_LIBS} tortoise )
target_link_libraries( netpeek ${OpenCV_LIBS} temporalnn nnvisualizer tortoise )
