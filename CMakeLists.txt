cmake_minimum_required(VERSION 3.17)

project(ArduinoLED VERSION 1.0
		   DESCRIPTION "ArduinoLED"
		   LANGUAGES CXX)

add_subdirectory(ArduinoControl)
add_subdirectory(ArduinoLED)
target_link_libraries(ArduinoLEDTest PUBLIC libArduinoControl)
