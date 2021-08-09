
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

devenv simple7zppwrapper.sln /build "Debug|x64"
devenv simple7zppwrapper.sln /build "Release|x64"