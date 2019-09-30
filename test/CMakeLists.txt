cmake_minimum_required(VERSION 3.0.0)

add_executable(test-ssqll test-db.cpp test-stmt.cpp)
target_link_libraries(test-ssqll gtest gtest_main ssqll)

install(TARGETS test-ssqll RUNTIME DESTINATION bin)

add_test(
	NAME test-ssqll
	COMMAND test-ssqll
)
