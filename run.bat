@echo off
if not exist build/ ( mkdir build )
robocopy .\html .\build\html /MIR > nul
robocopy .\images .\build\images /MIR > nul
cd build
g++ ../server.c ../http_response.c ../file.c -o server.exe -lws2_32
server.exe