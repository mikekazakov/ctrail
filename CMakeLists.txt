cmake_minimum_required(VERSION 3.12)
project(ctrail)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_library(ctrail
  include/ctrail/Registry.h
  include/ctrail/RegistryImpl.h
  include/ctrail/RegistrySnapshot.h
  src/Registry.cpp
  src/RegistryImpl.cpp
)

target_compile_features(ctrail PUBLIC cxx_std_17)
target_compile_options(ctrail PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>: /W4>)
target_include_directories(ctrail 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()
add_subdirectory(test)
