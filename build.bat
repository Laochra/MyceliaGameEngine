@echo OFF
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
echo -------------------------------------------------
echo Starting Debug Build...
devenv "MyceliaGameEngine.sln" /build Debug
echo Debug Build Finished
echo -------------------------------------------------
echo Starting Release Build...
devenv "MyceliaGameEngine.sln" /build Release
echo Release Build Finished
echo -------------------------------------------------
echo All Builds Complete!
echo -------------------------------------------------
pause
