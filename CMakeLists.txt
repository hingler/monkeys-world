cmake_minimum_required(VERSION 3.15.0)
project(monkeys-world VERSION 0.0.1)

include(CTest)
enable_testing()

# linting

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

add_executable(monkeys-world main.cpp)

# separate out a bit better for testing
add_library(monkeys-world-components ${SRC_DIR}/storage/VertexPacketTypes.cpp
                                                  ${SRC_DIR}/shader/ShaderProgramBuilder.cpp
                                                  ${SRC_DIR}/shader/ShaderProgram.cpp
                                                  ${SRC_DIR}/shader/materials/MatteMaterial.cpp
                                                  ${SRC_DIR}/file/CacheStreambuf.cpp
                                                  ${SRC_DIR}/file/CachedFileLoader.cpp
                                                  ${SRC_DIR}/utils/FileUtils.cpp
                                                  ${SRC_DIR}/critter/Context.cpp
                                                  ${SRC_DIR}/critter/GameObject.cpp
                                                  ${SRC_DIR}/utils/IDGenerator.cpp
                                                  ${SRC_DIR}/critter/Model.cpp)

set_property(TARGET monkeys-world-components PROPERTY CXX_STANDARD 14)

target_include_directories(monkeys-world-components PUBLIC ${INC_DIR})

### GLFW ###

find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(monkeys-world-components glfw)

### GLAD ###
find_package(glad CONFIG REQUIRED)

target_link_libraries(monkeys-world-components glad::glad)

### FREETYPE ###
find_package(freetype CONFIG REQUIRED)

### GTEST ###
find_package(GTest CONFIG REQUIRED)

### GLM ###
find_package(glm CONFIG REQUIRED)

### BOOST ###
set(BOOST_USE_MULTITHREADED ON)
set(BOOST_USE_STATIC_LIBS ON)
set(Boost_DEBUG 1)
find_package(Boost 1.72.0 COMPONENTS log REQUIRED)

# this should be all i need for now
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

target_link_libraries(monkeys-world-components glm glad::glad)
target_include_directories(monkeys-world-components PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(monkeys-world-components ${Boost_LIBRARIES})

target_link_libraries(monkeys-world monkeys-world-components)

add_executable(mesh-test test/MeshTest.cpp)
target_include_directories(mesh-test PRIVATE ${INC_DIR})
target_link_libraries(mesh-test GTest::gtest_main GTest::gmock_main monkeys-world-components)
target_include_directories(mesh-test PRIVATE ${Boost_INCLUDE_DIRS})
add_test(NAME mesh-test COMMAND mesh-test WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world>)

add_executable(shader-compile-test test/ShaderCompilationTest.cpp)
target_include_directories(shader-compile-test PRIVATE ${INC_DIR})
target_link_libraries(shader-compile-test GTest::gtest_main GTest::gmock_main monkeys-world-components)
add_test(NAME shader-compile-test COMMAND shader-compile-test
            WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world>)

add_executable(mat-test test/demos/SimpleMatTest.cpp)
target_include_directories(mat-test PRIVATE ${INC_DIR})
target_link_libraries(mat-test glad::glad glm monkeys-world-components)

add_executable(streambuf-test test/CacheStreambufTest.cpp)
target_include_directories(streambuf-test PRIVATE ${INC_DIR})
target_link_libraries(streambuf-test GTest::gtest_main monkeys-world-components)
add_test(NAME streambuf-test COMMAND streambuf-test
            WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world>)

add_custom_command(TARGET mat-test POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:monkeys-world>/resources)

add_executable(loader-test test/CacheFileLoaderTest.cpp)
target_include_directories(loader-test PRIVATE ${INC_DIR})
target_link_libraries(loader-test GTest::gtest_main monkeys-world-components)
add_test(NAME loader-test COMMAND loader-test
            WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world>)

add_executable(generator-test test/IDGeneratorTest.cpp)
target_include_directories(generator-test PRIVATE ${INC_DIR})
target_link_libraries(generator-test GTest::gtest_main monkeys-world-components)
add_test(NAME generator-test COMMAND generator-test
            WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world>)

add_executable(object-test test/GameObjectTest.cpp)
target_include_directories(object-test PRIVATE ${INC_DIR})
target_link_libraries(object-test GTest::gtest_main monkeys-world-components)
add_test(NAME object-test COMMAND object-test
            WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world>)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 