cmake_minimum_required (VERSION 3.8)

project(crate)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_SOURCE_DIR}/cmake/SetEnv.cmake)

include_directories($CMAKE_SOURCE_DIR/crate)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/crate/externals/aixlog/logger.hpp
    ${CMAKE_SOURCE_DIR}/crate/externals/simplejson/json.hpp
    ${CMAKE_SOURCE_DIR}/crate/common/common.hpp
    ${CMAKE_SOURCE_DIR}/crate/registrar/cache.hpp
)

set(SOURCES
   ${CMAKE_SOURCE_DIR}/crate/externals/simplejson/json.cpp
   ${CMAKE_SOURCE_DIR}/crate/common/common.cpp
   ${CMAKE_SOURCE_DIR}/crate/registrar/cache.cpp
)

include(${CMAKE_SOURCE_DIR}/cmake/LibraryConfig.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/InstallConfig.cmake)
