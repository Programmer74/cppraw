cmake_minimum_required(VERSION 2.7)
project(cppraw)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0) # look into FindPkgConfig.cmake,
# it contains documentation
# Now the variables GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES
# contain what you expect

add_subdirectory(src)