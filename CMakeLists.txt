cmake_minimum_required(VERSION 3.16)

project(bsa_extractor)

find_package(mo2-cmake CONFIG REQUIRED)

mo2_configure_extension()

add_subdirectory(src)
