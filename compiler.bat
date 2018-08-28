@echo off

:compile
cls
echo ==============================
bam client_release
echo ==============================
echo press any key to compile agian...
pause >NUL
goto compile