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
if not exist "_bin\debug" mkdir "_bin\debug"

cl.exe /nologo /EHsc /c /Zi /W3 /WX /MTd /std:c++17 /Fd"_bin\debug\vc141.pdb" /Fo"_bin\debug\nx.obj" "src\nx.c" /I"src" /I"kore/include" /DWIN32 /D_DEBUG
link.exe /nologo /OUT:"_bin\debug\nx.exe" /WX /DEBUG:FULL /PDB:"_bin\debug\nx.pdb" /SUBSYSTEM:WINDOWS /MACHINE:X64 "_bin\debug\nx.obj" gdi32.lib user32.lib
