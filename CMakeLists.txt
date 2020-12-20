cmake_minimum_required(VERSION 3.16)
project(ngf VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

# options
option(NGF_BUILD_EXAMPLES "Build examples" ON)
option(NGF_BUILD_DEMOS "Build demos" ON)
option(NGF_BUILD_TESTS "Build tests" ON)
option(NGF_BUILD_DOCUMENTATION "Build documentation (needs Doxygen)" OFF)

add_subdirectory(extlibs)
add_subdirectory(src)

## add exta compilation flags
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -pedantic -Wall -Wextra)
endif ()

# Build examples
if (NGF_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# Build demos
if (NGF_BUILD_DEMOS)
    add_subdirectory(demos)
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
install(TARGETS ${PROJECT_NAME} imgui
EXPORT ngf-export
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib)

# Create a package
## Configure CPack
function(get_short_system_name variable)
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(${variable} "OSX" PARENT_SCOPE)
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        set(${variable} "win64" PARENT_SCOPE)
    else ()
        set(${variable} "${CMAKE_SYSTEM_NAME}" PARENT_SCOPE)
    endif ()
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
