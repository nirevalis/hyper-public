add_executable(Server
    ${RUNTIME_SOURCES}
)

target_compile_features(Server PRIVATE cxx_std_20)

target_compile_definitions(Server PRIVATE
    HYPER_PLAYER_USE_DLL
    SERVER
)
target_sources(Server PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Runtime/main.cpp)
target_link_libraries(Server PRIVATE Game)
