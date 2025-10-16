cmake_minimum_required(VERSION 3.24)
project(EpiKodi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Générations automatiques Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 requis (Widgets pour l'UI, Multimedia pour le player, MultimediaWidgets pour QVideoWidget)
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets Network Sql)

# nlohmann/json (fetch propre, prêt pour la M2/M3)
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Sources / headers
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.hpp)

add_executable(EpiKodi
  ${SOURCES}
  ${HEADERS}
)

target_include_directories(EpiKodi PRIVATE include)

target_link_libraries(EpiKodi
  PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Network
    Qt6::Sql
    nlohmann_json::nlohmann_json
)

# Copie des assets à côté de l’exécutable (utile sous Windows)
if (WIN32)
  add_custom_command(TARGET EpiKodi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/assets"
      "$<TARGET_FILE_DIR:EpiKodi>/assets"
  )
endif()
