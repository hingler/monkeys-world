# todo: this looks like a mess and VS2017 doesn't like it.
# do some comment cleanup here lol

cmake_minimum_required(VERSION 3.15.0)
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

set_property(TARGET screenspace-window-manager-components PROPERTY CXX_STANDARD 14)

target_include_directories(screenspace-window-manager-components PUBLIC ${INC_DIR})

### GLFW ###

find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(screenspace-window-manager-components glfw)

### GLAD ###
find_package(glad CONFIG REQUIRED)

target_link_libraries(screenspace-window-manager-components glad::glad)

### FREETYPE ###
find_package(freetype CONFIG REQUIRED)
target_link_libraries(screenspace-window-manager-components freetype)

### GTEST ###
find_package(GTest CONFIG REQUIRED)

### GLM ###
find_package(glm CONFIG REQUIRED)

### BOOST ###
set(BOOST_USE_MULTITHREADED ON)
set(BOOST_USE_STATIC_LIBS ON)
set(Boost_DEBUG 1)
find_package(Boost 1.72.0 COMPONENTS log REQUIRED)

target_link_libraries(screenspace-window-manager-components glm glad::glad)
target_include_directories(screenspace-window-manager-components PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(screenspace-window-manager-components ${Boost_LIBRARIES})

target_link_libraries(screenspace-window-manager screenspace-window-manager-components)

add_executable(mesh-test test/MeshTest.cpp)
target_include_directories(mesh-test PRIVATE ${INC_DIR})
target_link_libraries(mesh-test GTest::gtest_main GTest::gmock_main screenspace-window-manager-components) # ${Boost_LIBRARIES}
target_include_directories(mesh-test PRIVATE ${Boost_INCLUDE_DIRS})
add_test(NAME mesh-test COMMAND mesh-test WORKING_DIRECTORY $<TARGET_FILE_DIR:screenspace-window-manager>)

add_executable(shader-compile-test test/ShaderCompilationTest.cpp)
target_include_directories(shader-compile-test PRIVATE ${INC_DIR})
target_link_libraries(shader-compile-test GTest::gtest_main GTest::gmock_main screenspace-window-manager-components)
add_test(NAME shader-compile-test COMMAND shader-compile-test
            WORKING_DIRECTORY $<TARGET_FILE_DIR:screenspace-window-manager>)

add_executable(mat-test test/demos/SimpleMatTest.cpp)
target_include_directories(mat-test PRIVATE ${INC_DIR})
target_link_libraries(mat-test glad::glad glm screenspace-window-manager-components)

add_custom_command(TARGET mat-test POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:screenspace-window-manager>/resources)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 