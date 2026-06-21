set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(Runtime main.cpp)

target_link_libraries(Runtime PRIVATE
    Game      
    nvrhi   
    SDL2::SDL2 
    glfw
)

target_compile_definitions(Runtime PRIVATE HYPER_PLAYER_USE_DLL)
