
all:: SDDS.ps SDDS.pdf html

SDDS.ps:
	rm -f SDDS.aux SDDS.dvi SDDS.log SDDS.toc
	latex SDDS
	latex SDDS
	dvips SDDS -o SDDS.ps

SDDS.pdf: SDDS.ps
	ps2pdf13 SDDS.ps SDDS.pdf

html:
	latex2html -local_icons SDDS

clean::
	rm -f SDDS.aux SDDS.dvi SDDS.log SDDS.toc SDDS.ps SDDS.pdf
	rm -rf SDDS