cmake_minimum_required(VERSION 3.0.0)

set(SOURCES 
	sqlite++.cpp
	${CMAKE_SOURCE_DIR}/include/sqlite++/sqlite++.h)

add_library(sqlite++ ${SOURCES})
target_include_directories(sqlite++ PRIVATE ${CMAKE_SOURCE_DIR}/include/sqlite++)
target_include_directories(sqlite++ PRIVATE ${CMAKE_SOURCE_DIR}/build/sqlite-src)


install(TARGETS sqlite++ 
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
