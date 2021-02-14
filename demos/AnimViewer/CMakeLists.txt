find_package(SDL2 CONFIG REQUIRED)

add_executable(AnimViewer AnimViewer.cpp AnimEditor.cpp)
if (WIN32)
    target_link_libraries(AnimViewer SDL2::SDL2main ngf)
else()
    target_link_libraries(AnimViewer ngf)
endif()
install(TARGETS AnimViewer DESTINATION "bin/demos")
