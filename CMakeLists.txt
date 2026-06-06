cmake_minimum_required(VERSION 3.28)
project(strategia LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SFML)

add_executable(strategia main.cpp Game.cpp States.cpp TestState.cpp Tile.hpp)
target_compile_features(strategia PRIVATE cxx_std_17)
target_link_libraries(strategia PRIVATE SFML::Graphics SFML::Window SFML::System)