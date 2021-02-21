list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake/SDL2")

find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)

set(LUA_SOURCE_FILES
        lua-5.4.1/src/lapi.c
        lua-5.4.1/src/lauxlib.c
        lua-5.4.1/src/lbaselib.c
        lua-5.4.1/src/lcode.c
        lua-5.4.1/src/lcorolib.c
        lua-5.4.1/src/lctype.c
        lua-5.4.1/src/ldblib.c
        lua-5.4.1/src/ldebug.c
        lua-5.4.1/src/ldo.c
        lua-5.4.1/src/ldump.c
        lua-5.4.1/src/lfunc.c
        lua-5.4.1/src/lgc.c
        lua-5.4.1/src/linit.c
        lua-5.4.1/src/liolib.c
        lua-5.4.1/src/llex.c
        lua-5.4.1/src/lmathlib.c
        lua-5.4.1/src/lmem.c
        lua-5.4.1/src/loadlib.c
        lua-5.4.1/src/lobject.c
        lua-5.4.1/src/lopcodes.c
        lua-5.4.1/src/loslib.c
        lua-5.4.1/src/lparser.c
        lua-5.4.1/src/lstate.c
        lua-5.4.1/src/lstring.c
        lua-5.4.1/src/lstrlib.c
        lua-5.4.1/src/ltable.c
        lua-5.4.1/src/ltablib.c
        lua-5.4.1/src/ltm.c
        lua-5.4.1/src/lundump.c
        lua-5.4.1/src/lutf8lib.c
        lua-5.4.1/src/lvm.c
        lua-5.4.1/src/lzio.c)

add_executable(LuaSample ${LUA_SOURCE_FILES} ImGuiColorTextEdit/TextEditor.cpp Graphics.cpp DemoApplication.cpp main.cpp)
if (MSVC)
    target_compile_options(LuaSample PRIVATE /W4 /Zc:twoPhase-)
endif ()
target_link_libraries(LuaSample ngf ${FREETYPE_LIBRARIES})
install(TARGETS LuaSample DESTINATION "bin/demos")

include_directories(./)
include_directories("lua-5.4.1/src")
include_directories("ImGuiColorTextEdit")
install(FILES "../../examples/assets/PressStart2P-vaV7.ttf"
        "../../examples/assets/background.jpg"
        DESTINATION "bin/demos/assets")
