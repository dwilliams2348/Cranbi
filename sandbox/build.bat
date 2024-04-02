REM Build script for sandbox
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET assembly=sandbox
SET compilerFlags=-g 
REM -Wall -Werror
SET includeFlags=-Isource -I../engine/source/
SET linkerFlags=-L../bin/ -lengine.lib
SET defines=-D_DEBUG -DCIMPORT

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%