cmake_minimum_required(VERSION 2.8)
project( CAUSATIONLIB )

add_executable( timequeue-test timequeue-test.cc )
add_executable( tortoise-test tortoise-test.cc )

target_link_libraries( tortoise-test tortoise )
target_link_libraries( timequeue-test tortoise )
