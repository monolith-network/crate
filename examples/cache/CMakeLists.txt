cmake_minimum_required(VERSION 3.8)

project(cacheApp)

set(CMAKE_CXX_STANDARD 20)

#
# Find Crate
#
find_package(crate REQUIRED)
include_directories(${CRATE_INCLUDE_DIRS})

#
# Find Threads
#
find_package (Threads REQUIRED)

#
#  Ensure that tld includes available everywhere
#
include_directories(
   ${CMAKE_SOURCE_DIR})

#
#  Create the app
#
add_executable(cacheApp
               main.cpp)

#
#  Link 
#
target_link_libraries(cacheApp
        PRIVATE
        ${CRATE_LIBRARIES}
        Threads::Threads
        )