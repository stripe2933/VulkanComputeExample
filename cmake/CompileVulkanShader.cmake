# Compile shader files and copy them into build directory.
#
# Usage:
# compile_shader(target
#     FORMAT spv
#     SOURCES path1/shader1.vert path2/shader2.frag ...
# )
#
# Result: path1/shader1.vert.spv, path2/shader2.frag.spv, ... in build directory.

find_package(Vulkan COMPONENTS glslc REQUIRED)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

function(compile_shader target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "FORMAT" "SOURCES")
    foreach(source ${arg_SOURCES})
        add_custom_command(
            OUTPUT ${source}.${arg_FORMAT}
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND ${glslc_executable} ${CMAKE_CURRENT_SOURCE_DIR}/${source} -o ${CMAKE_CURRENT_BINARY_DIR}/${source}.${arg_FORMAT}
        )
        target_sources(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${source}.${arg_FORMAT})
    endforeach()
endfunction()