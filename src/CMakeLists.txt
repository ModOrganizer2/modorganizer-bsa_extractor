cmake_minimum_required(VERSION 3.16)

add_library(bsa_extractor SHARED)
mo2_configure_plugin(bsa_extractor
    WARNINGS OFF
    PRIVATE_DEPENDS bsatk)
target_include_directories(bsa_extractor PUBLIC ${DDS_ROOT})
mo2_install_target(bsa_extractor)
