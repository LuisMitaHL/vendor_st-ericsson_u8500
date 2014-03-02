@echo off
set doxygen_dir=%~dp0
set latex_output_dir=..\target\doc\latex\

cd %doxygen_dir%
mkdir %latex_output_dir%

doxygen

copy template\eusdoc2.sty %latex_output_dir%
copy template\elogo.pdf %latex_output_dir%

cd %latex_output_dir%

pdflatex refman.tex
makeindex refman.idx
pdflatex refman.tex
pdflatex refman.tex

cd %doxygen_dir%
pause
