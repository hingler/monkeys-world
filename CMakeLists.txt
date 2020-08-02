cmake_minimum_required(VERSION 3.0.0)
project(screenspace-window-manager VERSION 0.1.0)

include(CTest)
enable_testing()

# linting

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

add_executable(screenspace-window-manager main.cpp)
add_library(screenspace-window-manager-components ${SRC_DIR}/Font.cpp)

target_include_directories(screenspace-window-manager-components PUBLIC ${INC_DIR})

### GLFW ###
configure_file(deps/CMakeLists-glfw.txt glfw-download/CMakeLists.txt)

# it looks like these lines are here to ensure that linters and shit work properly.
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glfw-download )

if(result)
  message(FATAL_ERROR "CMake configuration for GLFW failed -- ${result}" )
endif()

# build it
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glfw-download )

if(result)
  message(FATAL_ERROR "Build for GLFW failed -- ${result}" )
endif()

# the external project add should build those files in those
# directories, so we add both now

#if problems arise add build dirs to subdirectory commands :-)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glfw-src
  EXCLUDE_FROM_ALL)

target_link_libraries(screenspace-window-manager-components glfw)

### GLAD ###
configure_file(deps/CMakeLists-glad.txt glad-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glad-download )

if(result)
  message(FATAL_ERROR "CMake configuration for GLAD failed -- ${result}" )
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glad-download )

if(result)
  message(FATAL_ERROR "Build for GLAD failed -- ${result}" )
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glad-src
  EXCLUDE_FROM_ALL)

target_link_libraries(screenspace-window-manager-components glad)

### FREETYPE ###
configure_file(deps/CMakeLists-freetype.txt freetype-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/freetype-download )

if(result)
  message(FATAL_ERROR "CMake configuration for Freetype failed -- ${result}" )
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/freetype-download )

if (result)
  message(FATAL_ERROR "Build for Freetype failed -- ${result}" )
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/freetype-src
  EXCLUDE_FROM_ALL)

target_link_libraries(screenspace-window-manager-components freetype)

### GTEST ###
configure_file(deps/CMakeLists-gtest.txt googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

if(result)
  message(FATAL_ERROR "CMake configuration for gtest failed -- ${result}" )
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

if (result)
  message(FATAL_ERROR "Build for gtest failed -- ${result}" )
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

target_link_libraries(screenspace-window-manager screenspace-window-manager-components)



add_custom_command(TARGET screenspace-window-manager POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:screenspace-window-manager>/resources)

# file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/resources)
# file(COPY "resources" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 