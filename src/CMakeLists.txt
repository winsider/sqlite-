cmake_minimum_required(VERSION 3.0.0)

set(SOURCES 
	ssqll.cpp
	${CMAKE_SOURCE_DIR}/include/ssqll/ssqll.h)

add_library(ssqll ${SOURCES})
target_include_directories(ssqll PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(ssqll PRIVATE ${CMAKE_SOURCE_DIR}/build/sqlite-src)
target_link_libraries(ssqll sqlite3)
set_property(TARGET ssqll PROPERTY CXX_STANDARD 14)

install(TARGETS ssqll 
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
