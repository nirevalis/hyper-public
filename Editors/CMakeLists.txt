set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(Editors
    Source/main.cpp
    Source/Lodepng/lodepng.hpp
    Source/Lodepng/lodepng.cpp
    Source/TinyGLTF/tiny_gltf.hpp
)

target_link_libraries(Editors PRIVATE
    Game
    stb_image
)

target_compile_definitions(Runtime PRIVATE HYPER_PLAYER_USE_DLL)
