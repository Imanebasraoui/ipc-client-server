@echo off
echo Compiling server...
gcc .\src\server.c -o .\bin\server -lws2_32 
echo Compiling client...
gcc .\src\client.c -o .\bin\client -lws2_32
start .\bin\client.exe
.\bin\server.exe
