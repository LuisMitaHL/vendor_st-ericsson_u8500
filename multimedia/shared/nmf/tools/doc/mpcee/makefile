DOCUMENT=mpcee

SRC = $(wildcard *.tex)

all: $(DOCUMENT).pdf

%dvi: $(SRC)
	latex $(DOCUMENT).tex $(DOCUMENT).dvi

%.pdf: $(SRC)
	pdflatex --enable-write18 $(DOCUMENT).tex $(DOCUMENT).pdf
	pdflatex --enable-write18 $(DOCUMENT).tex $(DOCUMENT).pdf

clean:
	rm -f *.aux *.log *.pdf *.toc *.out *.dvi