@echo off
if not exist build/ ( mkdir build )
robocopy .\resources .\build\resources /MIR > nul
cd build
g++ ../src/server.c ../src/http_response.c ../src/file.c -o server.exe -Iinclude -Lws2_32
server.exe