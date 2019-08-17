cmake_minimum_required(VERSION 3.0.0)

add_executable(test-ssqll test-db.cpp)
include_directories(${CMAKE_SOURCE_DIR}/include/ssqll)
include_directories(${CMAKE_SOURCE_DIR}/build/sqlite-src)
target_link_libraries(test-ssqll sqlite3 gtest gtest_main)

install(TARGETS test-ssqll RUNTIME DESTINATION bin)

add_test(
	NAME test-ssqll
	COMMAND test-ssqll
)
