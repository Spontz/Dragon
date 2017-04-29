REM VC14 X86_32

rmdir caopsia_vc_14_x86_32_shared_unicode /s /q
mkdir caopsia_vc_14_x86_32_shared_unicode
cd caopsia_vc_14_x86_32_shared_unicode
start cmake.exe -G "Visual Studio 14" -DBUILD_SHARED_LIBS:BOOL=TRUE -DBUILD_UNICODE:BOOL=TRUE ..\..\
cd..

rmdir caopsia_vc_14_x86_32_static_unicode /s /q
mkdir caopsia_vc_14_x86_32_static_unicode
cd caopsia_vc_14_x86_32_static_unicode
start cmake.exe -G "Visual Studio 14" -DBUILD_SHARED_LIBS:BOOL=FALSE -DBUILD_UNICODE:BOOL=TRUE ..\..\
cd..



REM VC14 X86_64

rmdir caopsia_vc_14_x86_64_shared_unicode /s /q
mkdir caopsia_vc_14_x86_64_shared_unicode
cd caopsia_vc_14_x86_64_shared_unicode
start cmake.exe -G "Visual Studio 14 Win64" -DBUILD_SHARED_LIBS:BOOL=TRUE -DBUILD_UNICODE:BOOL=TRUE ..\..\
cd..

rmdir caopsia_vc_14_x86_64_static_unicode /s /q
mkdir caopsia_vc_14_x86_64_static_unicode
cd caopsia_vc_14_x86_64_static_unicode
start cmake.exe -G "Visual Studio 14 Win64" -DBUILD_SHARED_LIBS:BOOL=FALSE -DBUILD_UNICODE:BOOL=TRUE ..\..\
cd..