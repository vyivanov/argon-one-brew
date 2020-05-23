cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(argon-one-salt)

add_executable(${CMAKE_PROJECT_NAME} argon-one-salt.cpp)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -std=c++17 -Wall -Werror)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE pthread)


install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
