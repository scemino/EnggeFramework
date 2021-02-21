list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake/SDL2")

find_package(SDL2 REQUIRED)

add_executable(WimpyViewer WimpyViewer.cpp Object.cpp)
target_link_libraries(WimpyViewer ngf)
install(TARGETS WimpyViewer DESTINATION "bin/demos")
