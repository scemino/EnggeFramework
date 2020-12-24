# dependencies
find_package(GLEW REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)
if (NOT WIN32)
    find_package(OpenGL REQUIRED)
endif ()

set(NGLIB_HEADERS_DIR
        ${OPENGL_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/extlibs/stb
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/extlibs/imgui
        ${PROJECT_SOURCE_DIR}/extlibs)

# ngf sources
set(SYS_SOURCE_FILES
        System/Cursor.cpp
        System/DisplayMode.cpp
        System/Mouse.cpp
        System/SdlSystem.cpp
        System/StopWatch.cpp
        System/StringHelper.cpp
        System/Times.cpp
        System/TimeSpan.cpp
        System/Util.cpp
        System/VideoDisplay.cpp)
set(WIN_SOURCE_FILES System/Window.cpp)
set(GFX_SOURCE_FILES
        Graphics/AnimatedSprite.cpp
        Graphics/Animation.cpp
        Graphics/CircleShape.cpp
        Graphics/Color.cpp
        Graphics/DefaultFont.cpp
        Graphics/FntFont.cpp
        Graphics/GlDebug.cpp
        Graphics/Image.cpp
        Graphics/ImGuiExtensions.cpp
        Graphics/RectangleShape.cpp
        Graphics/RenderTarget.cpp
        Graphics/RenderTexture.cpp
        Graphics/Shader.cpp
        Graphics/Shape.cpp
        Graphics/Sprite.cpp
        Graphics/Text.cpp
        Graphics/Texture.cpp
        Graphics/VertexArray.cpp
        Graphics/VertexBuffer.cpp
        Graphics/View.cpp)
set(IO_SOURCE_FILES
        IO/GGPackHashReader.cpp
        IO/GGPack.cpp
        IO/GGPackBufferStream.cpp
        IO/MemoryStream.cpp
        IO/Json/JsonTokenReader.cpp
        IO/Json/JsonParser.cpp)
set(MATH_SOURCE_FILES
        Math/Segment.cpp
        Math/Transform.cpp
        Math/PathFinding/AStarAlgorithm.cpp
        Math/PathFinding/IndexedPriorityQueue.cpp
        Math/PathFinding/Graph.cpp
        Math/PathFinding/GraphEdge.cpp
        Math/PathFinding/PathFinder.cpp
        Math/PathFinding/Walkbox.cpp)
set(APP_SOURCE_FILES Application.cpp)
set(SOURCE_FILES ${APP_SOURCE_FILES} ${IO_SOURCE_FILES} ${SYS_SOURCE_FILES} ${WIN_SOURCE_FILES} ${GFX_SOURCE_FILES} ${MATH_SOURCE_FILES}
        # SDL2+OpenGL-specific files
        ../extlibs/imgui/examples/imgui_impl_sdl.cpp
        ../extlibs/imgui/examples/imgui_impl_opengl3.cpp)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${NGLIB_HEADERS_DIR})
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_17)
# if debug -> add DEBUG preprocessor
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

## For Linux only, to use std::filesystem
if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLM_LIBRARIES} GLEW::GLEW imgui stdc++fs)
else()
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2::SDL2 ${OPENGL_LIBRARIES} ${GLM_LIBRARIES} GLEW::GLEW imgui)
endif ()

