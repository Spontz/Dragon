REM VS15 X86_32

rmdir sve_vs15_x86_32 /s /q
mkdir sve_vs15_x86_32
cd sve_vs15_x86_32
start cmake.exe -G "Visual Studio 15" -DBUILD_SHARED_LIBS:BOOL=FALSE -DBUILD_UNICODE:BOOL=TRUE ..\
cd..


REM VS15 X86_64

rmdir sve_vs15_x86_64 /s /q
mkdir sve_vs15_x86_64
cd sve_vs15_x86_64
start cmake.exe -G "Visual Studio 15 Win64" -DBUILD_SHARED_LIBS:BOOL=FALSE -DBUILD_UNICODE:BOOL=TRUE ..\
cd..