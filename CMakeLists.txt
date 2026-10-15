cmake_minimum_required(VERSION 3.22)

project(Fledge VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version")
endif()

# Path to the JUCE checkout (the .jucer used ~/JUCE). Override with -DJUCE_DIR=...
set(JUCE_DIR "$ENV{HOME}/JUCE" CACHE PATH "Path to the JUCE source tree")
add_subdirectory(${JUCE_DIR} JUCE EXCLUDE_FROM_ALL)

set(FLEDGE_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND FLEDGE_FORMATS AU)
endif()

juce_add_plugin(Fledge
    COMPANY_NAME "Rainbow Circuit"
    COMPANY_WEBSITE "www.RainbowCircuit.com"
    PLUGIN_MANUFACTURER_CODE Manu   # 0x4d616e75
    PLUGIN_CODE Hewo                # 0x4865776f
    FORMATS ${FLEDGE_FORMATS}
    PRODUCT_NAME "Fledge"
    BUNDLE_ID com.RainbowCircuit.Fledge
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES Instrument Synth
    AU_MAIN_TYPE kAudioUnitType_MusicDevice)

# Sources use #include <JuceHeader.h>; generate it for the CMake build
juce_generate_juce_header(Fledge)

file(GLOB FLEDGE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
target_sources(Fledge PRIVATE ${FLEDGE_SOURCES})
target_include_directories(Fledge PRIVATE Source)

target_compile_definitions(Fledge PUBLIC
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(Fledge
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
