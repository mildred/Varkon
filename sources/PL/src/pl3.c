/*!******************************************************************/
/*  File: pl3.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   plrast();   Vector to pixel (raster) conversion                */
/*                                                                  */
/*  This file is part of the VARKON Plotter Library.                */
/*  URL:  http://www.varkon.com                                     */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/PL.h"
#include <string.h>

static short putpix();        /* S�tter bit i bit-mappen */

extern char *bmbpek[];        /* Pekare till bitmap-block */
extern short bitmsx,bitmsy;   /* Rastrets storlek i x och y */

/*!******************************************************/

        short plrast(
        short ix1,
        short iy1,
        short ix2,
        short iy2)

/*      Rsterkonverterar vektor. Bitmappens nedre v�nstra
 *      h�rn �r (0,0) och storleken �r bitmsx X bitmsy.
 *
 *      In: ix1,iy1 => Vektors startpos.
 *          ix2,iy2 => Vektors slutpos.
 *
 *      Ut: Rasterkonverterar och lagrar resultatet
 *          i bitmap.
 *
 *      FV: Inget. 
 *
 *     (C)microform ab 30/12/88 Johan Kjellander
 *      
 ******************************************************!*/

{
   short  dx,dy,tmp,i,ixack,iyack;
   double rst,rstsum;

/*
***Se till att vektorn g�r fr�n v�nster till h�ger och
***ber�kna vektorns utstr�ckning i X- och Y-led.
*/
   if ( ix1 > ix2 )
     {
     tmp = ix1; ix1 = ix2; ix2 = tmp;
     tmp = iy1; iy1 = iy2; iy2 = tmp;
     }

   dx = ix2 - ix1;
   dy = iy2 - iy1;
/*
***Specialfallet horisontell vektor.
*/
   if ( dy == 0 )
     for ( i=0; i<dx+1; ++i) putpix(ix1+i,iy1);
/*
***Specialfallet vertikal vektor.
*/
   else if ( dx == 0 )
     if ( dy > 0 ) 
       for ( i=0; i<dy+1; ++i) putpix(ix1,iy1+i);
     else
       for ( i=dy; i<1; ++i) putpix(ix1,iy1+i);
/*
***Specialfallet 45-graders vektor snett upp�t.
*/
   else if ( dx == dy )
     for ( i=0; i<dx+1; ++i) putpix(ix1+i,iy1+i);
/*
***Specialfallet 45-graders vektor snett ned�t.
*/
   else if ( dx == -dy )
     for ( i=0; i<dx+1; ++i) putpix(ix1+i,iy1-i);
/*
***Vektor riktad mer �n 45 grader snett upp�t.
*/
   else if ( dy > dx )
     {
     rst = dx; rst = rst/dy; /* Tal mellan 0 och 1 */
     rstsum = 0.0;
     ixack = ix1;           /* Startv�rde f�r x */
     putpix(ix1,iy1);       /* 1:a pixeln */
     for ( i=1; i<dy; ++i)
       {
       rstsum += rst;
       if ( rstsum > 0.5 ) { ++ixack; rstsum -= 1.0; }
       putpix(ixack,iy1+i);
       }
     putpix(ix2,iy2);       /* Sista pixeln */
     }
/*
***Vektor riktad mindre �n 45 grader snett upp�t.
*/
   else if ( dy > 0 )
     {
     rst = dy; rst = rst/dx; /* Tal mellan 0 och 1 */
     rstsum = 0.0;
     iyack = iy1;           /* Startv�rde f�r y */
     putpix(ix1,iy1);       /* 1:a pixeln */
     for ( i=1; i<dx; ++i)
       {
       rstsum += rst;
       if ( rstsum > 0.5 ) { ++iyack; rstsum -= 1.0; }
       putpix(ix1+i,iyack);
       }
     putpix(ix2,iy2);       /* Sista pixeln */
     }
/*
***Vektor riktad mindre �n 45 grader snett ned�t.
*/
   else if ( dy > -dx )
     {
     rst = -dy; rst = rst/dx; /* Tal mellan 0 och 1 */
     rstsum = 0.0;
     iyack = iy1;           /* Startv�rde f�r y */
     putpix(ix1,iy1);       /* 1:a pixeln */
     for ( i=1; i<dx; ++i)
       {
       rstsum += rst;
       if ( rstsum > 0.5 ) { --iyack; rstsum -= 1.0; }
       putpix(ix1+i,iyack);
       }
     putpix(ix2,iy2);       /* Sista pixeln */
     }
/*
***Vektor riktad mer �n 45 grader snett ned�t.
*/
   else
     {
     rst = dx; rst = rst/(-dy); /* Tal mellan 0 och 1 */
     rstsum = 0.0;
     ixack = ix1;           /* Startv�rde f�r x */
     putpix(ix1,iy1);       /* 1:a pixeln */
     for ( i=1; i<(-dy); ++i)
       {
       rstsum += rst;
       if ( rstsum > 0.5 ) { ++ixack; rstsum -= 1.0; }
       putpix(ixack,iy1-i);
       }
     putpix(ix2,iy2);       /* Sista pixeln */
     }
/*
***Slut.
*/
   return(0);

}

/********************************************************/
/*!******************************************************/

        static short putpix(ix,iy)
        short ix,iy;

/*      T�nder pixel i bitmapp.
 *
 *      In: ix,iy => Pixeladress, (0,0) = nedre v�nstra
 *                   h�rnet = C-adressen bitmpk och bit
 *                   nummer 0 (LSB) i denna byte.
 *
 *                   Pixeladress (1,0) motsvarar C-adress
 *                   bitmpk+1 och bit nummer 0 i denna byte.
 *
 *                   Pixeladress (1,1) motsvarar C-adress
 *                   bitmpk+1 och bit nummer 1 i denna byte.
 *
 *      Ut: Inget.
 *
 *      FV: Inget. 
 *
 *     (C)microform ab 30/12/88 Johan Kjellander
 *
 *     26/1/89 Blockad bitmapp, J. Kjellander
 *      
 ******************************************************!*/

{
   long  bytofs;
   short block;
   char *bytadr;
   char  bytmsk;
   int   intmsk;
   
/*
***Klipptest f�rst av allt.
*/
   if ( ix < 0  ||  ix >= bitmsx ) return(0);
   if ( iy < 0  ||  iy >= bitmsy ) return(0);
/*
***Ber�kna den ber�rda bytens offset fr�n 1:a bitmap-blockets
***b�rjan.
*/
   bytofs = ((long)bitmsx)*(iy>>3) + ix;
/*
***Ber�kna vilket block vi hamnar i och offset inom blocket.
*/
   block = (short)(bytofs/BMBSIZ);
   bytofs -= (long)block*(long)BMBSIZ;
/*
***Ber�kna C-adress.
*/
   bytadr = bmbpek[block] + bytofs;
/*
***Skapa mask med r�tt bit satt. iy&7 �r den bit inom
***byten som ber�rs.
*/
   intmsk = 1;
   bytmsk = (char)(intmsk<<(iy&7));
/*
***S�tt motsvarande bit i bitmap-byten.
*/
   *bytadr = (*bytadr) | (bytmsk);
/*
***Slut.
*/
   return(0);
}

/********************************************************/
