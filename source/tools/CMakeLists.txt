
# Check if examples are enabled
if(NOT OPTION_BUILD_TOOLS)
    return()
endif()

# Example applications
add_subdirectory(featurecheck)
add_subdirectory(renderingcheck)
