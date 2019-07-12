@echo off
set string=%~1
set string=%string:/=\%
type %string%
