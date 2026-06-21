# Game / CMakeLists.txt

include(ShaderUtils.cmake)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Color.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Float2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Float3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Float4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Math.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Math.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Matrix4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Palette.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Quaternion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Rectangle.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Transform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Math/Transform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Serialization/Stream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Serialization/Stream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/String/WideString.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/String/StringUtil.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/String/Strings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/String/String.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/GUID.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/GUID.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Log.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Macros.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Event.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Asset/Asset.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Asset/Asset.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Asset/AssetRegistry.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Asset/AssetRegistry.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/Event.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/Event.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/GameEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/GameEngine.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/Layer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/LayerStack.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/LayerStack.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Entities/PlayerEntity.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Entities/PlayerEntity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Entities/StaticModelEntity.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Entities/StaticModelEntity.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Entities/FallingEntity.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Entities/FallingEntity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/LocalPlayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/LocalPlayer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/IMap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/IMap.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Maps/GymMap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Maps/GymMap.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Game.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Game/Game.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Async/TextureUploadTask.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Assets/TextureAsset.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Assets/TextureAsset.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Assets/ModelAsset.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Assets/ModelAsset.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Devices/D3D12DeviceImpl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Devices/D3D12DeviceImpl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Pipelines/RayTracingPipeline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Pipelines/RayTracingPipeline.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Pipelines/RasterPipeline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Pipelines/RasterPipeline.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Pipelines/HybridStyledPipeline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Pipelines/HybridStyledPipeline.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Camera.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/FramebufferCache.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/FramebufferCache.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/SceneRenderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/SceneRenderer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/ScriptablePipeline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/ScriptablePipeline.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Skybox.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Skybox.hpp"
    "Source/Renderer/GPUTask.cpp"
    "Source/Renderer/GPUTask.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/ShaderLibrary.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/ShaderLibrary.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/DescriptorTableManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/DescriptorTableManager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Render2D.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Render2D.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/PSOCache.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/PSOCache.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Mesh.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/DescriptorsCache.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/DescriptorsCache.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/ImGui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/ImGui.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/DrawContext.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Material.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/JobManager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/JobManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/Task.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/Task.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/TaskExecutor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/TaskExecutor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/TaskGraph.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/TaskGraph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Threading/TaskExecutorGroup.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scene/Entity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scene/Entity.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scene/EntityPool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scene/EntityPool.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scene/Scene.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scene/Scene.hpp"
        Source/Game/GameWindow.hpp
        Source/Game/GameWIndow.cpp
        Source/Renderer/Async/TextureDownloadTask.hpp
        Source/Engine/EventDispatcher.hpp
        Source/Engine/EventDispatcher.cpp
        Source/Game/UserInterface.hpp
        Source/Game/UserInterface.cpp
        Source/Game/GameRenderer.hpp
        Source/Game/GameRenderer.cpp
        Source/Renderer/Font.hpp
        Source/Renderer/Font.cpp
        Source/Physics/PhysicsScene.hpp
        Source/Physics/PhysicsScene.cpp
        Source/Physics/JoltMath.hpp
        Source/Game/ActionSystem.cpp
        Source/Game/ActionSystem.hpp
        Source/Physics/BodyDataStorage.hpp
        Source/Game/Entities/HouseDoorEntity.hpp
        Source/Game/Entities/HouseDoorEntity.cpp
        Source/Game/Weapon/WeaponSystem.hpp
        Source/Game/Weapon/WeaponSystem.cpp
        Source/Game/Weapon/BulletEntity.hpp
        Source/Game/Weapon/BulletEntity.cpp

)

add_library(Game SHARED ${FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" FILES ${FILES})

target_compile_definitions(Game PRIVATE HYPER_PLAYER_BUILD_DLL)

target_include_directories(Game PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_include_directories(Game PRIVATE "${enet_SOURCE_DIR}/include")

target_link_libraries(Game PUBLIC spdlog::spdlog enet nvrhi nvrhi_d3d12 stb_image imgui glfw msdf-atlas-gen::msdf-atlas-gen Jolt::Jolt)

file(GLOB shaders
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.hlsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.hlsli"
)

set(DONUT_WITH_DX12 ON)
set(DXC_PATH "C:/DXC/bin/x64/dxc.exe")
#set(SLANGC_PATH "C:/slang/bin/slangc.exe")

donut_compile_shaders(
    TARGET Shaders
    CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/shaders.cfg
    SOURCES ${shaders}    DXIL ${CMAKE_CURRENT_SOURCE_DIR}/Compiled
    SHADER_MODEL 6_6
)

add_dependencies(Game Shaders)