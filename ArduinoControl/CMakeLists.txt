cmake_minimum_required(VERSION 3.17)

set(ControlSourceDir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ControlIncludeDir ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB ControlSources ${ControlSourceDir}/*.cpp)
file(GLOB ControlHeaders ${ControlIncludeDir}/*.h)

add_library(libArduinoControl STATIC ${ControlSources} ${ControlHeaders})
set_property(TARGET libArduinoControl PROPERTY CXX_STANDARD 20)

target_include_directories(libArduinoControl PUBLIC ${ControlIncludeDir})
