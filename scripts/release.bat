
REM ---------------------------------------------------------------------------
REM Batch file for implementing release flow for cpp-vapory for Windows.
REM
REM The documentation for cpp-vapory is hosted at http://cpp-vapory.co
REM
REM ---------------------------------------------------------------------------
REM This file is part of cpp-vapory.
REM
REM cpp-vapory is free software: you can redistribute it and/or modify
REM it under the terms of the GNU General Public License as published by
REM the Free Software Foundation, either version 3 of the License, or
REM (at your option) any later version.
REM
REM cpp-vapory is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU General Public License for more details.
REM
REM You should have received a copy of the GNU General Public License
REM along with cpp-vapory.  If not, see <http://www.gnu.org/licenses/>
REM
REM Copyright (c) 2016 cpp-ethereum contributors.
REM ---------------------------------------------------------------------------

set CONFIGURATION=%1
set VAPORY_DEPS_PATH=%2
set TESTS=%3

7z a cpp-vapory-windows.zip ^
    .\build\vap\%CONFIGURATION%\vap.exe ^
    .\build\vapkey\%CONFIGURATION%\vapkey.exe ^
    .\build\vapvm\%CONFIGURATION%\vapvm.exe ^
    .\build\rlp\%CONFIGURATION%\rlp.exe ^
    .\build\test\%CONFIGURATION%\testvap.exe ^
    "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\x86\Microsoft.VC140.CRT\msvc*.dll"
