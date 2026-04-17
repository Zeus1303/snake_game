cmake_minimum_required(VERSION 3.16)
project(SnakeGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Tìm SFML (yêu cầu >= 3.0) ──────────────────────────────────────
# Nếu SFML được cài ở thư mục không mặc định, truyền:
#   -DSFML_DIR="C:/path/to/SFML/lib/cmake/SFML"
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# ── Tạo executable ──────────────────────────────────────────────────
add_executable(${PROJECT_NAME} main.cpp)

# ── Link SFML ────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# ── Copy font vào build directory (tuỳ chọn) ────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/arial.ttf")
    configure_file(
        "${CMAKE_SOURCE_DIR}/arial.ttf"
        "${CMAKE_BINARY_DIR}/arial.ttf"
        COPYONLY
    )
endif()

# ── Windows: copy DLL của SFML vào cùng thư mục exe ─────────────────
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()
