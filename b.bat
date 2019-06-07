@echo off

set "folder=%cd%"

if "%VSINSTALLDIR%"=="" (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat" (
            call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
        )
    )
)

cd /d %folder%

if not exist "_bin" mkdir "_bin"
if not exist "_obj" mkdir "_obj"
if not exist "_bin\debug" mkdir "_bin\debug"
if not exist "_obj\debug" mkdir "_obj\debug"

cl.exe /nologo /EHsc /c /Zi /W3 /WX /MTd /std:c++17 /Fd"_obj\debug\vc141.pdb" /Fo"_obj\debug\nx.obj" "src\nx.c" /I"src" /DWIN32 /D_DEBUG
link.exe /nologo /OUT:"_bin\debug\nx.exe" /WX /DEBUG:FULL /PDB:"_bin\debug\bin2c.pdb" /SUBSYSTEM:WINDOWS /MACHINE:X64 "_obj\debug\nx.obj"
