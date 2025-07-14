cmake_minimum_required(VERSION 3.15)
project(SDLMinigame)

set(CMAKE_CXX_STANDARD 17)

# Buscar SDL2 en tu sistema
find_package(SDL2 REQUIRED)

# Agregar el directorio fuente
add_executable(SDLMinigame src/main.cpp)

# Incluir los headers de SDL2
target_include_directories(SDLMinigame PRIVATE ${SDL2_INCLUDE_DIRS})

# Vincular las librerías de SDL2
target_link_libraries(SDLMinigame PRIVATE ${SDL2_LIBRARIES})