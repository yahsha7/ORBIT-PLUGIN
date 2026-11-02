cmake_minimum_required(VERSION 3.25)

project(ORBIT VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ORBIT_JUCE_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." CACHE PATH
    "Path to the JUCE source tree. Leave empty to use an installed JUCE package.")

if(ORBIT_JUCE_SOURCE_DIR AND EXISTS "${ORBIT_JUCE_SOURCE_DIR}/modules/juce_audio_processors")
    add_subdirectory("${ORBIT_JUCE_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/JUCE" EXCLUDE_FROM_ALL)
else()
    find_package(JUCE CONFIG REQUIRED)
endif()

set(ORBIT_PLUGIN_FORMATS VST3 Standalone)

juce_add_plugin(ORBIT
    COMPANY_NAME "Orbit Instruments"
    BUNDLE_ID "com.orbitinstruments.orbit"
    PLUGIN_MANUFACTURER_CODE Orbt
    PLUGIN_CODE Or01
    FORMATS ${ORBIT_PLUGIN_FORMATS}
    PRODUCT_NAME "ORBIT"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(ORBIT
    PRIVATE
        plugin/Parameters.cpp
        plugin/PluginProcessor.cpp)

target_include_directories(ORBIT PRIVATE plugin)

target_compile_definitions(ORBIT
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(ORBIT
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

include(CTest)
option(ORBIT_BUILD_TESTS "Build ORBIT unit tests" ON)

if(BUILD_TESTING AND ORBIT_BUILD_TESTS)
    find_package(Catch2 3 CONFIG QUIET)

    if(NOT Catch2_FOUND)
        include(FetchContent)
        FetchContent_Declare(Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.8.1
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(Catch2)
    endif()

    add_executable(OrbitTests tests/PluginStateTests.cpp)
    target_include_directories(OrbitTests
        PRIVATE
            plugin
            "${ORBIT_JUCE_SOURCE_DIR}/modules")
    target_link_libraries(OrbitTests
        PRIVATE
            ORBIT
            Catch2::Catch2WithMain
            juce::juce_recommended_config_flags
            juce::juce_recommended_warning_flags)

    include(Catch)
    catch_discover_tests(OrbitTests)
endif()