cmake_minimum_required (VERSION 3.8)

project(crate)

set(CMAKE_CXX_STANDARD 20)

option(WITH_NETWORKING "Enable crate networking utils (required Nettle)" ON)

include(${CMAKE_SOURCE_DIR}/cmake/SetEnv.cmake)

include_directories($CMAKE_SOURCE_DIR/crate)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/crate/externals/aixlog/logger.hpp
    ${CMAKE_SOURCE_DIR}/crate/externals/simplejson/json.hpp
    ${CMAKE_SOURCE_DIR}/crate/externals/uuid4/uuid4.h
    ${CMAKE_SOURCE_DIR}/crate/common/common.hpp
    ${CMAKE_SOURCE_DIR}/crate/control/action_v1.hpp
    ${CMAKE_SOURCE_DIR}/crate/registrar/helper.hpp
    ${CMAKE_SOURCE_DIR}/crate/registrar/node_v1.hpp
    ${CMAKE_SOURCE_DIR}/crate/registrar/controller_v1.hpp
    ${CMAKE_SOURCE_DIR}/crate/metrics/reading_v1.hpp
    ${CMAKE_SOURCE_DIR}/crate/metrics/streams/helper.hpp
    ${CMAKE_SOURCE_DIR}/crate/metrics/streams/stream_data_v1.hpp
    ${CMAKE_SOURCE_DIR}/crate/metrics/streams/stream_receiver_if.hpp
)

set(SOURCES
   ${CMAKE_SOURCE_DIR}/crate/externals/simplejson/json.cpp
   ${CMAKE_SOURCE_DIR}/crate/externals/uuid4/uuid4.c
   ${CMAKE_SOURCE_DIR}/crate/common/common.cpp
   ${CMAKE_SOURCE_DIR}/crate/control/action_v1.cpp
   ${CMAKE_SOURCE_DIR}/crate/registrar/helper.cpp
   ${CMAKE_SOURCE_DIR}/crate/registrar/node_v1.cpp
   ${CMAKE_SOURCE_DIR}/crate/registrar/controller_v1.cpp
   ${CMAKE_SOURCE_DIR}/crate/metrics/streams/helper.cpp
   ${CMAKE_SOURCE_DIR}/crate/metrics/reading_v1.cpp
   ${CMAKE_SOURCE_DIR}/crate/metrics/streams/stream_data_v1.cpp
)

if (WITH_NETWORKING)
   find_package(Nettle REQUIRED)
   include_directories(${NETTLE_INCLUDE_DIRS})

   list(APPEND HEADERS 
      ${CMAKE_SOURCE_DIR}/crate/networking/message_receiver_if.hpp
      ${CMAKE_SOURCE_DIR}/crate/networking/message_server.hpp
      ${CMAKE_SOURCE_DIR}/crate/networking/message_writer.hpp)

   list(APPEND SOURCES 
      ${CMAKE_SOURCE_DIR}/crate/networking/message_server.cpp
      ${CMAKE_SOURCE_DIR}/crate/networking/message_writer.cpp)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/LibraryConfig.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/InstallConfig.cmake)
