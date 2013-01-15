
all:: SDDStoolkit.ps SDDStoolkit.pdf html

SDDStoolkit.ps:
	rm -f SDDStoolkit.aux SDDStoolkit.dvi SDDStoolkit.log SDDStoolkit.toc
	latex SDDStoolkit
	latex SDDStoolkit
	dvips SDDStoolkit -o SDDStoolkit.ps

SDDStoolkit.pdf: SDDStoolkit.ps
	ps2pdf13 SDDStoolkit.ps SDDStoolkit.pdf

html:
	latex2html -local_icons SDDStoolkit

clean::
	rm -f SDDStoolkit.aux SDDStoolkit.dvi SDDStoolkit.log SDDStoolkit.toc SDDStoolkit.ps SDDStoolkit.pdf
	rm -rf SDDStoolkit