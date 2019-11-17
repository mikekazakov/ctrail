cmake_minimum_required(VERSION 3.12)

project(ctrail)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package( Threads )

add_library(ctrail
  include/ctrail/Dashboard.h
  include/ctrail/DashboardImpl.h
  include/ctrail/Registry.h
  include/ctrail/RegistryImpl.h
  include/ctrail/RegistrySnapshot.h
  include/ctrail/ValuesStorage.h
  include/ctrail/MonotonicValuesStorage.h  
  include/ctrail/CyclicValuesStorage.h
  include/ctrail/ValuesStorageExporter.h
  include/ctrail/CSVExporter.h
  include/ctrail/ChromeTraceExporter.h
  include/ctrail/OneShotMonitor.h
  src/Dashboard.cpp
  src/DashboardImpl.cpp
  src/Registry.cpp
  src/RegistryImpl.cpp
  src/ValuesStorage.cpp
  src/MonotonicValuesStorage.cpp
  src/CyclicValuesStorage.cpp
  src/ValuesStorageExporter.cpp
  src/CSVExporter.cpp
  src/ChromeTraceExporter.cpp
  src/OneShotMonitor.cpp
)

target_compile_features(ctrail PUBLIC cxx_std_17)
target_compile_options(ctrail PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>: /W4>)
target_include_directories(ctrail 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries( ctrail ${CMAKE_THREAD_LIBS_INIT} )

enable_testing()
add_subdirectory(test)

add_subdirectory(examples)
