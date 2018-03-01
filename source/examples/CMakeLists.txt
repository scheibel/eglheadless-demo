
# Check if examples are enabled
if(NOT OPTION_BUILD_EXAMPLES)
    return()
endif()

# Example applications
add_subdirectory(gl-computation)
add_subdirectory(gles-cube)
add_subdirectory(vg-triangle)
