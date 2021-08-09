#!/bin/bash

./build_32.bat
./build_64.bat

rm -rf bin/

mkdir -p bin/Win32/Debug
cp Debug/*.lib bin/Win32/Debug
cp Debug/*.dll bin/Win32/Debug

mkdir -p bin/Win32/Release
cp Release/*.lib bin/Win32/Release
cp Release/*.dll bin/Win32/Release

mkdir -p bin/x64/Debug
cp x64/Debug/*.lib bin/x64/Debug
cp x64/Debug/*.dll bin/x64/Debug

mkdir -p bin/x64/Release
cp x64/Release/*.lib bin/x64/Release
cp x64/Release/*.dll bin/x64/Release

out=`date +"release_%Y%m%d_%H%M%S.7z"`
"C:\Program Files\7-Zip\7z.exe" a ${out} bin
