cmake_minimum_required(VERSION 2.6)
project(CAUSATIONLIB)
subdirs(src tests)

# allow dirs without CMakeLists.txt
cmake_policy( SET CMP0014 OLD )
