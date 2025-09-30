cmake_minimum_required(VERSION 3.10)

project(OPENGL_APP)

set(SRC_HEADERFILES )
set(SRC_SOURCEFILES )
set(SRC_MAIN src/main.cpp)
set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)

# add libralies
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)


add_executable(OPENGL_APP ${SRC_HEADERFILES} ${SRC_SOURCEFILES} ${SRC_MAIN})
set_target_properties(OPENGL_APP PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../src)

file(GLOB_RECURSE SimulationApp_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

file(GLOB_RECURSE EXTRA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/*/*.cpp"
)

target_sources(OPENGL_APP PRIVATE ${SimulationApp_SOURCES} ${EXTRA_SOURCES})

target_include_directories(OPENGL_APP PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(OPENGL_APP PUBLIC OpenMP::OpenMP_CXX)
endif()
target_link_libraries(OPENGL_APP PUBLIC
    glfw 
    glm::glm
    glad::glad
    Freetype::Freetype
    imgui::imgui
)


target_compile_options(OPENGL_APP PUBLIC
    -Wall
    -Wextra
    -Wpedantic
)