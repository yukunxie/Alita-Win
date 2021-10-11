

include_directories(
        ./
)

add_subdirectory(glslang)
add_subdirectory(SPIRV)
add_subdirectory(OGLCompilersDLL)

file(GLOB SRCS "./*.cpp")
file(GLOB HEADS "./*.h")

add_library( # Sets the name of the library.
        vulkan-lib-static

        # Sets the library as a shared library.
        STATIC

        ${HEADS}
        ${SRCS}
        )

target_link_libraries( # Specifies the target library.
        vulkan-lib-static
        glslang
        SPIRV
        OGLCompiler
        )