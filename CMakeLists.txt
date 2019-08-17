cmake_minimum_required(VERSION 3.0.0)
project(ssqll)

# Download and unpack sqlite at configure time
configure_file(sqlite-download.in sqlite-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sqlite-download )
if(result)
  message(FATAL_ERROR "CMake step for sqlite failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sqlite-download )
if(result)
  message(FATAL_ERROR "Build step for sqlite failed: ${result}")
endif()
configure_file(sqlite-build.in sqlite-src/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} ./../sqlite-src 
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sqlite-build )
if(result)
  message(FATAL_ERROR "Build step for sqlite failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sqlite-build )
if(result)
  message(FATAL_ERROR "Build step for sqlite failed: ${result}")
endif()

# Add sqlite directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/sqlite-src
                 ${CMAKE_CURRENT_BINARY_DIR}/sqlite-build
                 EXCLUDE_FROM_ALL)

# Download and unpack googletest at configure time
configure_file(gtest.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)

