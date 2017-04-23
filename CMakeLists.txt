#Application de test pour le driver de bus parallèle Virtu'Ailes
cmake_minimum_required(VERSION 2.8)

project(vpbus_test)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")

add_executable(
	vpbus_test
	main.cpp
)






