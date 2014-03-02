::GenerateDocument.bat
::Runs Doxygen to Create Documentation
::%1 = The path directory containing the code. This path is relative to SupportScripts directory.
::%2 = Name of the module.

:START

	::Doxygen Related Implementation Document Generation.

	::Now in the directory where DoxyFile is present
	PUSHD Latex_Detailed
		
	doxygen Doxyfile
	
	:: go into the Documentation\Interface directory
	CD .\..\..\..\Documentation\Implementation\latex\
	pdflatex refman.tex
	makeindex refman.idx
	pdflatex refman.tex
	copy refman.pdf .\..\Implementation.pdf
	CD ..
	rmdir .\latex /s /q

	
	:: return back to the support script directory
	POPD


	


:END
