cmake_minimum_required(VERSION 3.8)

project(stream)

set(CMAKE_CXX_STANDARD 20)

#
# Find Crate
#
find_package(crate REQUIRED)
include_directories(${CRATE_INCLUDE_DIRS})

find_package (Nettle REQUIRED)

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
add_executable(stream
               main.cpp)

#
#  Link 
#
target_link_libraries(stream
        PRIVATE
        ${CRATE_LIBRARIES}
        ${NETTLE_LIBRARIES}
        Threads::Threads
        )