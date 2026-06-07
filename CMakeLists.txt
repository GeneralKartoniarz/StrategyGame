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

add_executable(strategia 
    main.cpp 
    Game.cpp 
    States/States.cpp 
    States/TestState.cpp 
    Map/MapGenerator.cpp 
    UI/InputController.cpp
    UI/Label.cpp
    UI/Button.hpp
    UI/Button.cpp
    UI/GameInterface.cpp)
target_compile_features(strategia PRIVATE cxx_std_17)
target_link_libraries(strategia PRIVATE SFML::Graphics SFML::Window SFML::System)
target_include_directories(strategia PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Map
    ${CMAKE_CURRENT_SOURCE_DIR}/States
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)