@ECHO OFF
SET mypath=%~dp0..\
SET /p ver=<%mypath%build\VERSION

:: Generate build files
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DINTERPRETER_VERSION:STRING="%ver%" --preset="windows-release" -H%mypath% -G "Visual Studio 17 2022"

:: Build binary
cmake --build %mypath%out\build\windows-release -j 10 --config Release

:: Move examples
xcopy /E /I /Y %mypath%examples %mypath%bin\windows-release\examples

:: Zip file automatically for release
:: TODO