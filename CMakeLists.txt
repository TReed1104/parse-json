# Set the project name
project(cpp-json-parser)

# Set the CMake version
cmake_minimum_required(VERSION 3.0)

# Use Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

# Create globs for the headers and source to catch all the files.
file(GLOB header_files "src/*.h" "src/*.hpp")
file(GLOB src_files "src/*.c" "src/*.cpp")

ADD_EXECUTABLE(cpp-json-parser ${header_files} ${src_files})
TARGET_LINK_LIBRARIES(cpp-json-parser ${CONAN_LIBS})

# Set the C++ version
set (CMAKE_CXX_STANDARD 11)
# Set the project for VS to use.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "cpp-json-parser")

# Copy the content folder to the /build for VS to be able to find them.
FILE(COPY content/ DESTINATION content)
# Copy the content folder to the /build/bin so the exe can find them.
FILE(COPY content/ DESTINATION bin/content)
# Copy the content copy script into the /build folder, this is used by VS to copy the content across on build.
FILE(COPY "pre-build-Script.bat" DESTINATION .)

add_custom_command(TARGET cpp-json-parser PRE_BUILD COMMAND "pre-build-Script.bat")