cmake_minimum_required(VERSION 3.0.0)

set(SOURCES 
	ssqll.cpp
	${CMAKE_SOURCE_DIR}/include/ssqll/ssqll.h)

add_library(ssqll ${SOURCES})
target_include_directories(ssqll PRIVATE ${CMAKE_SOURCE_DIR}/include/ssqll)
target_include_directories(ssqll PRIVATE ${CMAKE_SOURCE_DIR}/build/sqlite-src)


install(TARGETS ssqll 
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
