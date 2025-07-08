cmake_minimum_required(VERSION 3.10)
project(MyCrowApp)

set(CMAKE_CXX_STANDARD 17)

find_package(Crow CONFIG REQUIRED)

# Include current directory for header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add more .cpp files if needed later
add_executable(MyCrowApp
    main.cpp
)

add_custom_command(TARGET MyCrowApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/UI
    ${CMAKE_CURRENT_BINARY_DIR}/UI
)

target_link_libraries(MyCrowApp PRIVATE Crow::Crow)
