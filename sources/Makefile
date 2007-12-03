#########################################
#
#   Makefile for Varkon version 1.19
#   --------------------------------
#
#   2007-11-28 S.Larsson 
#
##########################################



#=========================================================
# Build options:
#=========================================================

#Uncomment the platform you are running on
PLATFORM=linux
#PLATFORM=crayT3E
#PLATFORM=sgi
#PLATFORM=aix
#PLATFORM=hpux
#PLATFORM=sun
#PLATFORM=FreeBSD

# Uncomment the line below to include routines for Robot
# Application Protocol (RAP). Only on Linux platform.
# DV3RAP=TRUE

# Uncomment the line below to include ODBC support
# see also README_ODBC
# DV3ODBC=TRUE

# Uncomment the line below to include routines specific
# for Örebro University.
# DV3ORU=TRUE



#=========================================================
# End build options
#=========================================================

export DV3RAP
export DV3ODBC
export DV3ORU

.PHONY: distr debug

distr:   BUILDTARGET=distr
debug:   BUILDTARGET=debug

distr debug:

# create directories if needed
	mkdir -p ./IG/lib
	mkdir -p ./PM/lib
	mkdir -p ./EX/lib
	mkdir -p ./DB/lib
	mkdir -p ./WP/lib
	mkdir -p ./GE/lib
	mkdir -p ./AN/lib
	mkdir -p ./PL/lib
	mkdir -p ../bin
	mkdir -p ../tmp
	#mkdir -p ../pid
	#mkdir -p ../app
	mkdir -p ../man

# If SVN is used, update includefile with current SVNVERSION.
	./update_versionfile

# Make xvarkon
	@make -C IG -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C PM -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C EX -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C DB -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C WP -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C GE -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -f Makefile.$(PLATFORM)

# Make mbsc
	@make -C AN -C src -f Makefile.$(PLATFORM)

# Make plotter programs
	@make -C PL -C src -f Makefile.$(PLATFORM) all

# Make vlib
	@make -C Vlib -f Makefile

# Make man
	@make -C MAN -C src -f Makefile
           
clean:
	@make -C IG -C src -f Makefile.$(PLATFORM) clean
	@make -C PM -C src -f Makefile.$(PLATFORM) clean
	@make -C EX -C src -f Makefile.$(PLATFORM) clean
	@make -C DB -C src -f Makefile.$(PLATFORM) clean
	@make -C WP -C src -f Makefile.$(PLATFORM) clean
	@make -C GE -C src -f Makefile.$(PLATFORM) clean
	@make -C AN -C src -f Makefile.$(PLATFORM) clean
	@make -C MAN -C src -f Makefile clean
	@make -C Vlib -f Makefile clean
	rm -f -r ../bin

##########################################################
