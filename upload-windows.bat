@echo off
echo Uploading from Windows...
cd /d %~dp0
pio run --target upload
pause
