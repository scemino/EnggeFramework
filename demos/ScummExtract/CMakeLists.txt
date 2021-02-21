list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake/SDL2")

find_package(SDL2 REQUIRED)

add_executable(ScummExtract ScummExtract.cpp)
target_link_libraries(ScummExtract ngf)
install(TARGETS ScummExtract DESTINATION "bin/demos")
