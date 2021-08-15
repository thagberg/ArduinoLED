cmake_minimum_required(VERSION 3.17)

set(ArduinoTestSourceDir ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB ArduinoTestSources ${ArduinoTestSourceDir}/*.cpp)

add_executable(ArduinoLEDTest ${ArduinoTestSources})
set_propertY(TARGET ArduinoLEDTest PROPERTY CXX_STANDARD 20)
