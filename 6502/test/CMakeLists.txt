
cmake_minimum_required(VERSION 3.7)

project(  M6502Emu )

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
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

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# source for the test executable
set  (M6502_SOURCES
		"src/main_6502.cpp"
		"src/6502LoadRegisterTests.cpp"
		)
		
source_group("src" FILES ${M6502_SOURCES})
		
add_executable( M6502Test ${M6502_SOURCES} 	)
add_dependencies( M6502Test M6502Lib )
target_link_libraries(M6502Test gtest)
target_link_libraries(M6502Test M6502Lib)
