set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ../cmake/catch2)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v2.13.1)

FetchContent_MakeAvailable(Catch2)

add_executable(tests Tween.cpp)
target_link_libraries(tests Catch2::Catch2)
target_link_libraries(tests ngf)

include(CTest)
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(tests)
