cmake_minimum_required(VERSION 3.2)
project(utility C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-pthread -Weverything -Wdeclaration-after-statement -Wdate-time")
set(CMAKE_C_FLAGS "-Wall -pedantic -g")
add_library(utility SHARED
  src/logger.c
  src/queue.c
  src/lib_semaphore.c
  )

set_target_properties(utility PROPERTIES
        VERSION 1.0
        SOVERSION 1
        PUBLIC_HEADER src/utility.h
        )

target_include_directories(utility PRIVATE src/)
include(GNUInstallDirs)

install(TARGETS utility
        LIBRARY DESTINATION /usr/lib/
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


