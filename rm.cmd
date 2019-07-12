@echo off
set string=%*
set string=%string:/=\%
del /Q %string%