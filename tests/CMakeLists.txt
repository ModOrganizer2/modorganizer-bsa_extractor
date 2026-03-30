add_executable(bsa_extractor-tests
  test_main.cpp
  test_archivehandlerdispatch.cpp
  ../src/archivehandlerdispatch.cpp
)

target_include_directories(bsa_extractor-tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(bsa_extractor-tests
  PRIVATE
    mo2::uibase
    GTest::gtest
)

add_test(NAME bsa_extractor-tests COMMAND bsa_extractor-tests)
