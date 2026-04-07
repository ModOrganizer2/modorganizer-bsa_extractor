cmake_minimum_required(VERSION 3.16)

project(bsa_extractor)

add_subdirectory(src)

include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
