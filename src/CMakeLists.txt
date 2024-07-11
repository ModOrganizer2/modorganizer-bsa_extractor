cmake_minimum_required(VERSION 3.16)

find_package(mo2-cmake CONFIG REQUIRED)
find_package(mo2-uibase CONFIG REQUIRED)
find_package(mo2-dds-header CONFIG REQUIRED)
find_package(mo2-bsatk CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(lz4 CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS thread interprocess)

add_library(bsa_extractor SHARED)
mo2_configure_plugin(bsa_extractor WARNINGS OFF)
target_link_libraries(bsa_extractor PRIVATE mo2::uibase mo2::bsatk)
mo2_install_plugin(bsa_extractor)
