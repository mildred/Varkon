##########################################################
#
#   Platform independant Makefile for Varkon plotterdrivers
#
#  (C)2008-01-12 J.Kjellander
#
##########################################################

# Include files

INCFILES = ../include/PL.h              \
           ../include/params.h          \
           ../../DB/include/DB.h        \
           ../../IG/include/IG.h        \

# The contents of PLlib.a

OBJECTS = ../lib/PLlib.a(pl1.o)         \
          ../lib/PLlib.a(pl2.o)         \
          ../lib/PLlib.a(pl3.o)         \
          ../lib/PLlib.a(pl4.o)         \
          ../lib/PLlib.a(pl5.o)         \
          ../lib/PLlib.a(pl6.o)

# Objects depend on sources implicitly and sources depend
# on the following include-files.

$(OBJECTS) : $(INCFILES)

# Each driver is a target

POSTSCRIPT: postscript.c $(OBJECTS) $(INCFILES)
	$(COMPILE) postscript.c ../lib/PLlib.a ../../IG/src/igenvpath.c -lm -o ../../../bin/postscript
	strip ../../../bin/postscript

HP7475: hp7475.c         $(OBJECTS) $(INCFILES)
	$(COMPILE) hp7475.c     ../lib/PLlib.a ../../IG/src/igenvpath.c -lm -o ../../../bin/hp7475
	strip ../../../bin/hp7475

HPCL5: hpcl5.c           $(OBJECTS) $(INCFILES)
	$(COMPILE) hpcl5.c      ../lib/PLlib.a ../../IG/src/igenvpath.c -lm -o ../../../bin/hpcl5
	strip ../../../bin/hpcl5

HPGL2 : hpgl2.c          $(OBJECTS) $(INCFILES)
	$(COMPILE) hpgl2.c      ../lib/PLlib.a ../../IG/src/igenvpath.c  -lm -o ../../../bin/hpgl2
	strip ../../../bin/hpgl2

# PLlib.a depends on the objects. 
PLLIB : $(OBJECTS)

clean:
	rm -f ../lib/PLlib.a

##########################################################