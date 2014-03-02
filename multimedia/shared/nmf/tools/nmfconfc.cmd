@ECHO OFF & SETLOCAL

SET TOOLSDIR=%~p0
SET CP=%TOOLSDIR%\nmftools.jar
SET LD=-Djava.library.path=%TOOLSDIR%

java -ea -cp %CP% %LD% tools.doelf %*
