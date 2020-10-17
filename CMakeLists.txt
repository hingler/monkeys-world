# todo: this looks like a mess and VS2017 doesn't like it.
# do some comment cleanup here lol

cmake_minimum_required(VERSION 3.18.0)
project(screenspace-window-manager VERSION 0.0.1)

include(CTest)
enable_testing()

# linting

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

add_executable(screenspace-window-manager main.cpp)

# separate out a bit better for testing
add_library(screenspace-window-manager-components ${SRC_DIR}/Font.cpp
                                                  ${SRC_DIR}/Paint.cpp
                                                  ${SRC_DIR}/utils/IDGenerator.cpp
                                                  ${SRC_DIR}/storage/VertexPacketTypes.cpp
                                                  ${SRC_DIR}/shader/ShaderProgramBuilder.cpp
                                                  ${SRC_DIR}/shader/ShaderProgram.cpp
                                                  ${SRC_DIR}/shader/materials/MatteMaterial.cpp)

target_include_directories(screenspace-window-manager-components PUBLIC ${INC_DIR})

### GLFW ###
configure_file(deps/CMakeLists-glfw.txt glfw-download/CMakeLists.txt)

# these download commands are fucked im pretty sure lmao

# it looks like these lines are here to ensure that linters and shit work properly.
# build works fine W/O some of the unnecessary steps (just the build folder)
# but the linter does not pick up on the build folders otherwise.
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

# if problems arise add build dirs to subdirectory commands :-)
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

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/freetype-src build
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

if(result)
  message(FATAL_ERROR "Build for gtest failed -- ${result}" )
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

### GLM ###
configure_file(deps/CMakeLists-glm.txt glm-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glm-download )

if(result)
  message(FATAL_ERROR "CMake configuration for glm failed -- ${result}" )
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glm-download )

if(result)
  message(FATAL_ERROR "Build for glm failed -- ${result}")
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glm-src
                 ${CMAKE_CURRENT_BINARY_DIR}/glm-build
                 EXCLUDE_FROM_ALL)

### BOOST ###

# BOOST_ROOT needs to be defined on my end but it might not need to be on yours :)
set(BOOST_USE_MULTITHREADED ON)
set(Boost_DEBUG 1)
set(BOOST_LIBRARYDIR "${BOOST_ROOT}")
# the fix: needed to build with prefix of ${BOOST_ROOT}
find_package(Boost 1.72.0 COMPONENTS log REQUIRED)
# download correct ver based on plat

target_link_libraries(screenspace-window-manager-components glm)
target_include_directories(screenspace-window-manager-components PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(screenspace-window-manager-components ${Boost_LIBRARIES})

target_link_libraries(screenspace-window-manager screenspace-window-manager-components)

add_executable(mesh-test test/MeshTest.cpp)
target_include_directories(mesh-test PRIVATE ${INC_DIR})
target_link_libraries(mesh-test gtest_main gmock_main glad glm screenspace-window-manager-components) # ${Boost_LIBRARIES}
target_include_directories(mesh-test PRIVATE ${Boost_INCLUDE_DIRS})
add_test(NAME mesh-test COMMAND mesh-test WORKING_DIRECTORY $<TARGET_FILE_DIR:screenspace-window-manager>)

add_executable(shader-compile-test test/ShaderCompilationTest.cpp)
target_include_directories(shader-compile-test PRIVATE ${INC_DIR})
target_link_libraries(shader-compile-test gtest_main gmock_main glad glm screenspace-window-manager-components)
add_test(NAME shader-compile-test COMMAND shader-compile-test
            WORKING_DIRECTORY $<TARGET_FILE_DIR:screenspace-window-manager>)

add_custom_command(TARGET screenspace-window-manager POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:screenspace-window-manager>/resources)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 