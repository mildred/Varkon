#####################################################################
#
#  This makefile preprocesses all htm-files in current directory
#  and move the resulting htm-files to corresponding directory 
#  under VARKON_ROOT/man
#
#  (c) 2007 Sören Larsson, Örebro University
#
#####################################################################

TARGETDIR = ../../../man
INCDIR    = ../include

PREPROCESS = cpp -P -nostdinc -I $(INCDIR)

INCLUDEFILES = ../include/header.inc \
               ../include/footer.inc \
               ../include/svnversion.inc

HTMSOURCES = $(wildcard *.htm)
HTMRESULT  = $(patsubst %.htm, $(TARGETDIR)/%.htm, $(HTMSOURCES))

CSSOURCES  = $(wildcard *.css)
CSSRESULT  = $(patsubst %.css, $(TARGETDIR)/%.css, $(CSSOURCES))

JPGSOURCES = $(wildcard *.jpg)
JPGRESULT  = $(patsubst %.jpg, $(TARGETDIR)/%.jpg, $(JPGSOURCES))

PNGSOURCES = $(wildcard *.png)
PNGRESULT  = $(patsubst %.png, $(TARGETDIR)/%.png, $(PNGSOURCES))

GIFSOURCES = $(wildcard *.gif)
GIFRESULT  = $(patsubst %.gif, $(TARGETDIR)/%.gif, $(GIFSOURCES))

all : $(HTMRESULT) $(CSSRESULT) $(JPGRESULT) $(PNGRESULT) $(GIFRESULT)
	@echo Creating subdirectories if needed
	mkdir -p ../../../man/m_man
	mkdir -p ../../../man/m_man/images
	mkdir -p ../../../man/v_man
	@echo make contents in subdirectories 
	@make -C m_man 
	@make -C m_man -C images
	@make -C v_man 

# preprocess htm-files, copy other files

$(TARGETDIR)/%.htm : ./%.htm $(INCLUDEFILES)
	$(PREPROCESS) $*.htm $(TARGETDIR)/$*.htm
                
$(TARGETDIR)/%.css : ./%.css
	cp $*.css $(TARGETDIR)/$*.css

$(TARGETDIR)/%.jpg : ./%.jpg
	cp $*.jpg $(TARGETDIR)/$*.jpg

$(TARGETDIR)/%.png : ./%.png
	cp $*.png $(TARGETDIR)/$*.png

$(TARGETDIR)/%.gif : ./%.gif
	cp $*.gif $(TARGETDIR)/$*.gif





