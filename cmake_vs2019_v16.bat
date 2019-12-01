REM VS16(2019) X86_64
rmdir dragon_vs16_x64 /s /q
mkdir dragon_vs16_x64
cd dragon_vs16_x64
start cmake.exe -G "Visual Studio 16 2019" -A x64 -DBUILD_SHARED_LIBS:BOOL=FALSE -DBUILD_UNICODE:BOOL=TRUE ..\
cd..