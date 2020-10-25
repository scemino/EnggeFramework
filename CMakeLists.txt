cmake_minimum_required(VERSION 3.17)
project(ngf)

set(CMAKE_CXX_STANDARD 17)

# dependencies
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
if (NOT WIN32)
    find_package(OpenGL REQUIRED)
endif ()

# include
set(NGLIB_HEADERS_DIR ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/extlibs/stb
        ${PROJECT_SOURCE_DIR}/include/ngf
        ${PROJECT_SOURCE_DIR}/include/ngf/Graphics
        ${PROJECT_SOURCE_DIR}/include/ngf/Window
        ${PROJECT_SOURCE_DIR}/include/ngf/System
        ${PROJECT_SOURCE_DIR}/extlibs/imgui)

include_directories(${NGLIB_HEADERS_DIR})

# source
set(SYS_SOURCE_FILES
        src/System/StopWatch.cpp
        src/System/TimeSpan.cpp
        src/System/Util.cpp)
set(WIN_SOURCE_FILES src/Window/Window.cpp)
set(GFX_SOURCE_FILES
        src/Graphics/AnimatedSprite.cpp
        src/Graphics/Animation.cpp
        src/Graphics/Color.cpp
        src/Graphics/FntFont.cpp
        src/Graphics/GlDebug.cpp
        src/Graphics/Image.cpp
        src/Graphics/RenderTarget.cpp
        src/Graphics/Shader.cpp
        src/Graphics/Sprite.cpp
        src/Graphics/Text.cpp
        src/Graphics/Texture.cpp
        src/Graphics/Transform.cpp
        src/Graphics/VertexArray.cpp
        src/Graphics/VertexBuffer.cpp)
set(APP_SOURCE_FILES src/Application.cpp)
set(SOURCE_FILES ${APP_SOURCE_FILES} ${AUDIO_SOURCE_FILES} ${IO_SOURCE_FILES} ${SYS_SOURCE_FILES} ${WIN_SOURCE_FILES} ${GFX_SOURCE_FILES}
        # SDL2+OpenGL-specific files
        extlibs/imgui/examples/imgui_impl_sdl.cpp
        extlibs/imgui/examples/imgui_impl_opengl3.cpp)

add_library(imgui
        # Main Imgui files
        extlibs/imgui/imgui.cpp extlibs/imgui/imgui_draw.cpp extlibs/imgui/imgui_widgets.cpp
        extlibs/imgui/misc/cpp/imgui_stdlib.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLM_LIBRARIES} GLEW::GLEW imgui)

# add exta compilation flags
target_compile_options(${PROJECT_NAME} PRIVATE -pedantic -Wall -Wextra)
# if debug -> add DEBUG preprocessor
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

add_subdirectory(examples)
