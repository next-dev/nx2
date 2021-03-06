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
if not exist "_bin\release" mkdir "_bin\release"

cl.exe /nologo /EHsc /c /Zi /W3 /WX /MT /Ox /std:c++17 /Fd"_bin\release\vc141.pdb" /Fo"_bin\release\nx.obj" "src\nx.c" /I"src" /DWIN32 /DNDEBUG
link.exe /nologo /OUT:"_bin\release\nx.exe" /WX /DEBUG:NONE /PDB:"_bin\release\nx.pdb" /SUBSYSTEM:WINDOWS /MACHINE:X64 "_bin\release\nx.obj"
