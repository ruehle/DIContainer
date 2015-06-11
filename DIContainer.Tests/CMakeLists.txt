include_directories(.)
add_executable(DIContainer.Tests DIContainer_tests.cpp main.cpp gmock-gtest-all.cc )
target_link_libraries(DIContainer.Tests pthread)
