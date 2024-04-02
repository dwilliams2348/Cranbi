#!/bin/bash
#Sandbox build script
set echo on

mkdir -p ../bin

#get a list of all .c files
cFilenames=$(find . -type f -name "*.c")

#echo "Files:" $cFilenames

assembly="sandbox"
compilerFlags="-g -fdeclspec -fPIC"
#-fms-extensions
#-Wall -Werror
includeFlags="-Isource -I../engine/source"
linkerFlags="-L../bin/ -lengine -Wl,-rpath,."
defines="-D_DEBUG -DCIMPORT"

echo "Building $assembly..."
echo clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags