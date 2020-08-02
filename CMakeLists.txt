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

# generate makefiles by running the cmake file in that directory
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glfw-download )

# result populated if fails
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
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glfw-src
  ${CMAKE_CURRENT_BINARY_DIR}/glfw-build
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
  ${CMAKE_CURRENT_BINARY_DIR}/glad-build
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

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/freetype-src)
target_link_libraries(screenspace-window-manager-components freetype)

target_link_libraries(screenspace-window-manager screenspace-window-manager-components)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 