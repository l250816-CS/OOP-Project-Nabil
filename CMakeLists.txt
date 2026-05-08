cmake_minimum_required(VERSION 3.16)

project(
  main
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(presentation)
add_subdirectory(business)

add_executable(TowerDefense src/main.cpp)

target_link_libraries(TowerDefense PRIVATE business)
target_link_libraries(TowerDefense PRIVATE presentation)

set_target_properties(
  TowerDefense
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
             VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:main>)

add_custom_command(
  TARGET TowerDefense
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:TowerDefense>/assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data
          $<TARGET_FILE_DIR:TowerDefense>/data)
