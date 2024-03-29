
# 
# External dependencies
# 

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../")

find_package(KHR REQUIRED)
find_package(EGL REQUIRED)
find_package(eglbinding REQUIRED)
find_package(glbinding REQUIRED)
find_package(glesbinding REQUIRED)

# 
# Executable name and options
# 

# Target name
set(target eglheadless-test)
message(STATUS "Test ${target}")


# 
# Sources
# 

set(sources
    main.cpp
    eglheadless_test.cpp
)


# 
# Create executable
# 

# Build executable
add_executable(${target}
    ${sources}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})


# 
# Project options
# 

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/source/include
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${META_PROJECT_NAME}::eglheadless
    glbinding::glbinding
    glbinding::glbinding-aux
    glesbinding::glesbinding
    glesbinding::glesbinding-aux
    googletest::googletest
)


# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
