cmake_minimum_required(VERSION 3.10)

project(SPH_FLUID_SIMULATION)

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



add_executable(SPH_FLUID_SIMULATION ${SRC_HEADERFILES} ${SRC_SOURCEFILES} ${SRC_MAIN})

file(GLOB_RECURSE SimulationApp_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

target_include_directories(SPH_FLUID_SIMULATION PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(SPH_FLUID_SIMULATION PUBLIC OpenMP::OpenMP_CXX)
endif()
target_link_libraries(SPH_FLUID_SIMULATION PUBLIC
    glfw 
    glm::glm
    glad::glad
    Freetype::Freetype
    imgui::imgui
)


target_compile_options(SPH_FLUID_SIMULATION PUBLIC
    -Wall
    -Wextra
    -Wpedantic
)
# cmake_minimum_required(VERSION 3.30)



# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# find_package(glm CONFIG REQUIRED)

# add_executable(SimulationApp)



# target_sources(SimulationApp PRIVATE ${SimulationApp_SOURCES})



# target_compile_definitions(SimulationApp PRIVATE
#     MY_TEST_DEFINE
# )


# target_link_libraries(SimulationApp PRIVATE
#     main PRIVATE glm::glm
# )

# .dlls or .libs


# # for vscode
