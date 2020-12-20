find_package(SDL2 CONFIG REQUIRED)

add_executable(ScummExtract ScummExtract.cpp)
if (WIN32)
    target_link_libraries(ScummExtract SDL2::SDL2main ngf)
else()
    target_link_libraries(ScummExtract ngf)
endif()
install(TARGETS ScummExtract DESTINATION "bin/demos")
