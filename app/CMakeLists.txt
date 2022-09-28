cmake_minimum_required(VERSION 3.22)

project(Example)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

set(PATH_TO_SFML ${ANDROID_NDK}/sources/third_party/sfml)

set(SFML_DIR ${PATH_TO_SFML}/lib/${ANDROID_ABI}/cmake/SFML)
find_package(SFML 3 COMPONENTS Graphics Audio Network REQUIRED)

set(PATH_TO_SOURCES src/main/cpp)
add_library(example SHARED
        ${PATH_TO_SOURCES}/main.cpp
        )
        
target_compile_features(example PUBLIC cxx_std_20)

add_library(openal SHARED IMPORTED)
set_target_properties(openal PROPERTIES IMPORTED_LOCATION_RELEASE ${PATH_TO_SFML}/extlibs/lib/${ANDROID_ABI}/libopenal.so)
set_target_properties(openal PROPERTIES IMPORTED_LOCATION_DEBUG ${PATH_TO_SFML}/extlibs/lib/${ANDROID_ABI}/libopenal.so)

target_link_libraries(example
        log
        android
        SFML::Graphics
        SFML::Audio
        SFML::Activity
        SFML::Network
        -Wl,--whole-archive SFML::Main -Wl,--no-whole-archive
        openal
        EGL
        GLESv1_CM
        )
