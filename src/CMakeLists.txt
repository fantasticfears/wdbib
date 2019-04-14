add_executable(wdbib main.cpp)
target_link_libraries(wdbib PRIVATE CLI11::CLI11 unofficial::abseil::strings ${Boost_LIBRARIES} fmt::fmt-header-only)
target_include_directories(wdbib PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(wdbib PUBLIC cxx_std_17)

add_executable(wdbib_test system_test.cpp)
target_link_libraries(wdbib_test PRIVATE CLI11::CLI11 unofficial::abseil::strings ${Boost_LIBRARIES} fmt::fmt-header-only Catch2::Catch2)
target_include_directories(wdbib_test PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(wdbib_test PUBLIC cxx_std_17)
