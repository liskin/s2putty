@echo off
echo Copying fonts to the emulator (epoc32\wins\c\system\apps\putty\fonts)
mkdir ..\..\..\epoc32\wins\c\system\apps\putty\fonts
copy ..\..\ui\s2font\*.s2f ..\..\..\epoc32\wins\c\system\apps\putty\fonts
