# CMakeList.txt : CMake project for codexter, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("codexter")

# Add source to this project's executable.
add_executable (codexter "src/codexter.cpp")

# Amend manifest to tell Windows that the application is DPI aware (needed for Windows 8.1 and up)
IF (MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Release")
	add_custom_command(
    TARGET codexter
    POST_BUILD
    COMMAND "mt.exe" -manifest \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\\codexter.exe.manifest\" -outputresource:"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\\codexter.exe"\;\#1
    COMMENT "Adding manifest..." 
  )
  add_custom_command(
    TARGET codexter
    POST_BUILD
    COMMAND "del" \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\\codexter.exe.manifest\"
    COMMENT "Deleting manifest..." 
  )
ENDIF(MSVC)
