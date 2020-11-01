cmake_minimum_required(VERSION 3.16)
project(ngf VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

# options
option(NGF_BUILD_EXAMPLES "Build examples" ON)
option(NGF_BUILD_TESTS "Build tests" ON)
option(NGF_BUILD_DOCUMENTATION "Build documentation (needs Doxygen)" OFF)

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
        ${PROJECT_SOURCE_DIR}/extlibs/imgui
        ${PROJECT_SOURCE_DIR}/extlibs)

include_directories(${NGLIB_HEADERS_DIR})

# source
set(SYS_SOURCE_FILES
        src/System/StopWatch.cpp
        src/System/StringHelper.cpp
        src/System/Times.cpp
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
        src/Graphics/VertexArray.cpp
        src/Graphics/VertexBuffer.cpp
        src/Graphics/View.cpp
        )
set(IO_SOURCE_FILES
        src/IO/GGPackHashReader.cpp
        src/IO/GGPack.cpp
        src/IO/GGPackBufferStream.cpp
        src/IO/MemoryStream.cpp
        src/IO/Json/JsonTokenReader.cpp
        src/IO/Json/JsonParser.cpp)
set(MATH_SOURCE_FILES
        src/Math/Segment.cpp
        src/Math/Transform.cpp
        src/Math/PathFinding/AStarAlgorithm.cpp
        src/Math/PathFinding/IndexedPriorityQueue.cpp
        src/Math/PathFinding/Graph.cpp
        src/Math/PathFinding/GraphEdge.cpp
        src/Math/PathFinding/PathFinder.cpp
        src/Math/PathFinding/Walkbox.cpp)
set(APP_SOURCE_FILES src/Application.cpp)
set(SOURCE_FILES ${APP_SOURCE_FILES} ${IO_SOURCE_FILES} ${SYS_SOURCE_FILES} ${WIN_SOURCE_FILES} ${GFX_SOURCE_FILES} ${MATH_SOURCE_FILES}
        # SDL2+OpenGL-specific files
        extlibs/imgui/examples/imgui_impl_sdl.cpp
        extlibs/imgui/examples/imgui_impl_opengl3.cpp)

# imgui library
add_library(imgui SHARED
        # Main Imgui files
        extlibs/imgui/imgui.cpp extlibs/imgui/imgui_draw.cpp extlibs/imgui/imgui_widgets.cpp
        extlibs/imgui/misc/cpp/imgui_stdlib.cpp)

# ngf library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLM_LIBRARIES} GLEW::GLEW imgui)
# For Linux only, to use std::filesystem
if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()

target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_17)
set_target_properties("${PROJECT_NAME}" PROPERTIES CXX_EXTENSIONS OFF)

# add exta compilation flags
target_compile_options(${PROJECT_NAME} PRIVATE -pedantic -Wall -Wextra)
# if debug -> add DEBUG preprocessor
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

# Build examples
if (NGF_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# Build documentation
if (NGF_BUILD_DOCUMENTATION)
    find_package(Doxygen)

    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target(doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif ()

# Build tests
if (NGF_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

# install ngf lib
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS imgui DESTINATION lib)

# Create a package
## Configure CPack
function(get_short_system_name variable)
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(${variable} "OSX" PARENT_SCOPE)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        set(${variable} "Win32" PARENT_SCOPE)
    else()
        set(${variable} "${CMAKE_SYSTEM_NAME}" PARENT_SCOPE)
    endif()
endfunction()

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
string(REPLACE " " "-" CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
get_short_system_name(CPACK_SYSTEM_NAME)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
string(TOLOWER "${CPACK_PACKAGE_FILE_NAME}" CPACK_PACKAGE_FILE_NAME)
set(CPACK_GENERATOR ZIP)

# Run CPack
include(CPack)
