@echo off
make clean
make
make program
rem powershell -c (New-Object Media.SoundPlayer "E:\Music\Samples\Hit1.wav").PlaySync();