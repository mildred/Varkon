/**********************************************************************
*
*    wpDECRN.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPcreate_3Dline();      Create a 3D line decoration
*    WPexpose_decoartion();  Expose decoration
*    WPdelete_decoration();  Delete a decoration
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"

/*!******************************************************/

        short  WPcreate_3Dline(
        int    pid,
        short  x1,
        short  y1,
        short  x2,
        short  y2)

/*      Create a 3D line decoration in a WPIWIN window.
 *
 *      In: pid   = Parent window ID.
 *          x1,y1 = Start position.
 *          x2,y2 = End position.
 *
 *      (C)2007-10-23 J. Kjellander
 *
 ******************************************************!*/

  {
   int      i;
   wpw_id   did;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPDECRN *dcrptr;

/*
***Get a C ptr to the WPIWIN.
*/
   winptr = WPwgwp(pid);
   iwinpt = (WPIWIN *)winptr->ptr;
/*
***Allocate an ID for the new decoration.
*/
   i = 0;
   while ( i < WP_IWSMAX  &&  iwinpt->wintab[i].ptr != NULL ) ++i;

   if ( i == WP_IWSMAX ) return(-3);
   else did = i;
/*
***Create a WPDECRN.
*/
   if ( (dcrptr=(WPDECRN *)v3mall(sizeof(WPDECRN),"WPcreate_decoration")) == NULL )
      return(-3);

   dcrptr->id.w_id = did;
   dcrptr->id.p_id = pid;
   dcrptr->id.x_id = iwinpt->id.x_id;

   dcrptr->type = LINE3DDECRN;

   dcrptr->x1 = x1;
   dcrptr->y1 = y1;
   dcrptr->x2 = x2;
   dcrptr->y2 = y2;
/*
***Add the decoration to the WPIWIN.
*/
   iwinpt->wintab[did].typ = TYP_DECRN;
   iwinpt->wintab[did].ptr = (char *)dcrptr;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool     WPexpose_decoration(
        WPDECRN *dcrptr)

/*      Expose handler for WPDECRN.
 *
 *      In: buttptr = C ptr to WPDECRN.
 *
 *      (C)2007-10-23 J.Kjellander
 *
 ******************************************************!*/

  {
   short dx,dy;

/*
***What type of decoration ?
*/
   switch ( dcrptr->type )
     {
/*
***A 3D line.
*/
     case LINE3DDECRN:
     XSetForeground(xdisp,xgc,WPgcol(WP_BOTS));
     XDrawLine(xdisp,dcrptr->id.x_id,xgc,dcrptr->x1,dcrptr->y1,
                                         dcrptr->x2,dcrptr->y2);
     XSetForeground(xdisp,xgc,WPgcol(WP_TOPS));

     dx = dcrptr->x2 - dcrptr->x1;
     dy = dcrptr->y2 - dcrptr->y1;

     if ( dy == 0 )
       XDrawLine(xdisp,dcrptr->id.x_id,xgc,dcrptr->x1,dcrptr->y1-1,
                                           dcrptr->x2,dcrptr->y2-1);
     else if ( dx == 0 )
       XDrawLine(xdisp,dcrptr->id.x_id,xgc,dcrptr->x1-1,dcrptr->y1,
                                           dcrptr->x2-1,dcrptr->y2);
     break;
     }
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        short    WPdelete_decoration(
        WPDECRN *dcrptr)

/*      Delete a WPDECRN.
 *
 *      In: buttptr = C ptr to WPDECRN.
 *
 *      (C)2007-10-23 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Release allocated C memory.
*/
   v3free((char *)dcrptr,"WPdelete_decoration");
/*
***The end.
*/
   return(0);
  }

/********************************************************/
