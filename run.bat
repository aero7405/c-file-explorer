@echo off
if not exist build/ ( mkdir build )
if not exist build\html/ ( mkdir build\html )
robocopy .\html .\build\html /MIR > nul
cd build
g++ ../server.c -o server.exe -lws2_32
server.exe