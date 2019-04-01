@echo off
set string=%*
IF %1==-p ( set string=%2 )
set string=%string:/=\%
IF NOT EXIST %string% ( mkdir %string% )