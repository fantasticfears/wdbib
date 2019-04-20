add_library(libwdbib
    system.cpp
    wdbib_data.cpp
    wikicite.cpp
    network/wikidata.cpp
    storage/data_file.cpp
    storage/spec_stateful_parser.cpp
)
target_link_libraries(libwdbib PRIVATE ssl crypto spdlog::spdlog CLI11::CLI11 nlohmann_json::nlohmann_json unofficial::abseil::strings ${Boost_LIBRARIES} fmt::fmt-header-only)
target_include_directories(libwdbib PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(libwdbib PUBLIC cxx_std_17)

add_executable(wdbib
    main.cpp
    commands/add.cpp
    commands/list.cpp
    commands/view.cpp
    commands/search.cpp
    commands/version.cpp
    commands/sync.cpp
    commands/convert.cpp
)
target_link_libraries(wdbib PRIVATE libwdbib spdlog::spdlog CLI11::CLI11 nlohmann_json::nlohmann_json unofficial::abseil::strings ${Boost_LIBRARIES} fmt::fmt-header-only)
target_include_directories(wdbib PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(wdbib PUBLIC cxx_std_17)

add_executable(wdbib_test
    system_test.cpp
    storage/spec_stateful_parser_test.cpp
)
# network/wikidata_test.cpp
# storage/data_file_test.cpp

target_link_libraries(wdbib_test PRIVATE libwdbib spdlog::spdlog CLI11::CLI11 nlohmann_json::nlohmann_json unofficial::abseil::strings ${Boost_LIBRARIES} fmt::fmt-header-only Catch2::Catch2)
target_include_directories(wdbib_test PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(wdbib_test PUBLIC cxx_std_17)
