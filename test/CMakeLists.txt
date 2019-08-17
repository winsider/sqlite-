cmake_minimum_required(VERSION 3.0.0)

add_executable(test-sqlite++ test-db.cpp)
include_directories(${CMAKE_SOURCE_DIR}/include/sqlite++)
include_directories(${CMAKE_SOURCE_DIR}/build/sqlite-src)
target_link_libraries(test-sqlite++ sqlite3 gtest gtest_main)

install(TARGETS test-sqlite++ RUNTIME DESTINATION bin)

add_test(
	NAME test-sqlite++
	COMMAND test-sqlite++
)
