# codexter
Codexter is an esolang that operates on a string of integers, with each digit 0-9 representing a different instruction. Concept by [@jsstenzel](https://github.com/jsstenzel).

## Usage
Run at the command line with the first argument either being a numeric string or a text file containing one or more delineated with whitespace.

## Building
Build with CMake using one of the supplied presets. Build scripts with properly formatted commands are in the `build` dir.
`windows-build.bat`: Builds using Visual Studio 2022 as the generator and moves the results to the `bin` dir.