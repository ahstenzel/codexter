# CMakeList.txt : CMake project for codexter, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("codexter")

# Add source to this project's executable.
add_executable (codexter "src/codexter.cpp")

# Pass through version string
if(DEFINED INTERPRETER_VERSION)
  add_compile_definitions(INTERPRETER_VERSION=\"${INTERPRETER_VERSION}\")
endif()