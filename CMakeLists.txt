cmake_minimum_required(VERSION 3.28)
project(strategia LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==============================================================================
# ZALEŻNOŚCI (SFML 3.1.0)
# ==============================================================================
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SFML)

# ==============================================================================
# PLIKI ŹRÓDŁOWE I NAGŁÓWKI
# ==============================================================================
add_executable(strategia 
    # Rdzeń gry
    main.cpp 
    Game.cpp 
    GameManager.hpp
    GameManager.cpp
    Unit.hpp
    PathFinder.hpp
    PathFinder.cpp
    Economy.hpp
    Industry.hpp
    Demographics.hpp

    # Moduł: Stany Gry
    States/States.cpp 
    States/MenuState.hpp
    States/MenuState.cpp
    States/TestState.cpp 

    # Moduł: Świat i Mapa (Map)
    Map/MapGenerator.cpp 
    Map/NameGenerator.cpp
    Map/VoronoiBuilder.hpp
    Map/VoronoiBuilder.cpp
    Map/ClimateEngine.hpp
    Map/ClimateEngine.cpp
    Map/NavigationGraph.hpp
    Map/MapRenderer.hpp
    Map/MapRenderer.cpp

    # Moduł: Ekonomia i Demografia (Empires)
    Empires/Pop.hpp
    Empires/PopManager.hpp
    Empires/PopManager.cpp
    Empires/City.hpp
    Empires/City.cpp
    Empires/Empire.hpp
    Empires/Empire.cpp
    Empires/PoliticalSetup.hpp
    Empires/PoliticalSetup.cpp

    # Moduł: Interfejs Użytkownika (UI)
    UI/InputController.cpp
    UI/Label.cpp
    UI/Button.hpp
    UI/Button.cpp
    UI/GameInterface.hpp
    UI/GameInterface.cpp
    UI/SidePanel.cpp
    UI/UnitPanel.hpp
    UI/UnitPanel.cpp
    UI/CityPanel.hpp
    UI/CityPanel.cpp
    UI/BuildPanel.hpp
    UI/BuildPanel.cpp
    random.hpp
)

# ==============================================================================
# KONFIGURACJA INTEGRACJI
# ==============================================================================
target_compile_features(strategia PRIVATE cxx_std_17)

# Włączenie ostrzeżeń kompilatora, żeby ułatwić wyłapywanie błędów logicznych
if(MSVC)
    target_compile_options(strategia PRIVATE /W4)
else()
    target_compile_options(strategia PRIVATE -Wall -Wextra)
endif()

target_link_libraries(strategia PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System
)

target_include_directories(strategia PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Map
    ${CMAKE_CURRENT_SOURCE_DIR}/States
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Empires
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

# ==============================================================================
# ZASOBY (Kopiowanie po kompilacji)
# ==============================================================================
add_custom_command(TARGET strategia POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    "$<TARGET_FILE_DIR:strategia>/resources"
)