list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake/SDL2")
find_package(SDL2 REQUIRED)

add_executable(AnimViewer AnimViewer.cpp AnimEditor.cpp)
target_link_libraries(AnimViewer ngf)
install(TARGETS AnimViewer DESTINATION "bin/demos")
