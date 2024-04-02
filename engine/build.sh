#!/bin/bash
#Engine build script
set echo on

mkdir -p ../bin

#get a list of all .c files
cFilenames=$(find . -type f -name "*.c")

#echo "Files:" $cFilenames

assembly="engine"
compilerFlags="-g -shared -fdeclspec -fPIC"
#-fms-extensions
#-Wall -Werror
includeFlags="-Isource -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -lxcb -lX11 -lX11-xcb -lxkbcommon -L$VULKAN_SDK/lib -Lusr/X11r6/lib"
defines="-D_DEBUG -DCEXPORT"

echo "Building $assembly..."
clang $cFilenames $compilerFlags -o ../bin/lib$assembly.so $defines $includeFlags $linkerFlags