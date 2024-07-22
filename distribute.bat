@echo OFF
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
echo Starting Build Process...
devenv "MyceliaGameEngine.sln" /build Release
echo -------------------------------------------------
echo All Builds Complete!
echo -------------------------------------------------
echo Cleaning Up Temporary Files...
echo -------------------------------------------------
attrib +h "Game\Editor.pdb"
attrib +h "Game\Engine.lib"
attrib +h "Game\Engine.pdb"
echo -------------------------------------------------
echo Temporary Files Cleaned Up! Ready to Distribute!
pause