@echo off
rem check command line arguments
set argC=0
for %%x in (%*) do Set /A argC+=1
echo %argC%

if not %argc%==1 goto invalid
if %argc%==1 goto valid

:valid
set vc_version=%1
echo "VC version: %vc_version%"
if "%vc_version%" == "VC10" goto copy_all
if "%vc_version%" == "VC11" goto copy_all
if "%vc_version%" == "VC12" goto copy_all
goto invalid_vc_version

:copy_all
copy externals\glog\vsprojects\libglog\%vc_version%\Debug\libglog.dll bin\Debug\
copy externals\glog\vsprojects\libglog\%vc_version%\Release\libglog.dll bin\Release\
goto end

:invalid
echo "Usage build_contrib.bat <vc_version [VC10|VC11|VC12]>"
goto end

:end
