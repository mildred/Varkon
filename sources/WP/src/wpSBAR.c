/**********************************************************************
*
*    wpSBAR.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPcreate_slidebar(); Create WPSBAR
*    WPexpose slidebar(); Expose WPSBAR
*    WPbutton_slidebar(); Button handler (scroll) for WPSBAR
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

        short    WPcreate_slidebar(
        int      pid,
        int      dir,
        WPSBAR **outptr)

/*      Creates a WPSBAR.
 *
 *      In: pid = Parent window ID.
 *          dir = WP_SBARH or WP_SBARV.
 *
 *      Out: *outptr = Pointer to WPSBAR.
 *
 *      Return:  0 = Ok.
 *              -3 = Programming error.
 *
 *      (C)2007-10-18 J. Kjellander
 *
 ******************************************************!*/

  {
    WPWIN               *winptr;
    WPLWIN              *lwinpt;
    Window               pxid,sbxid;
    int                  pdx,pdy,sx,sy,sdx,sdy;
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    WPSBAR              *sbptr;

/*
***Get a C pointer to the parent's entry in wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL ) return(-3);
/*
***Parent size etc.
*/
   if ( winptr->typ == TYP_LWIN )
     {
     lwinpt = (WPLWIN *)winptr->ptr;
     pxid   = lwinpt->id.x_id;
     pdx    = lwinpt->geo.dx;
     pdy    = lwinpt->geo.dy;
     }
   else return(-3);
/*
***Slidebar window position and size relative
***to parent window.
*/
    sx  = pdx - WPstrh();
    sy  = 0;
    sdx = WPstrh();
    sdy = pdy;
/*
***Create the X window.
*/
    xwina.background_pixel  = WPgcbu(WPwfpx(pxid),WP_BGND2);
    xwina.override_redirect = True;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel | CWOverrideRedirect | CWSaveUnder );

    sbxid = XCreateWindow(xdisp,pxid,sx,sy,sdx,sdy,0,CopyFromParent,
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***A slidebar window needs mouse input.
*/
    XSelectInput(xdisp,sbxid,ButtonPressMask | ButtonReleaseMask | ButtonMotionMask);
/*
***Crete the WPSBAR.
*/
    if ( (sbptr=(WPSBAR *)v3mall(sizeof(WPSBAR),"WPcreate_slidebar")) == NULL )
       return(-3);

    sbptr->id.w_id = (wpw_id)NULL;
    sbptr->id.p_id = pid;
    sbptr->id.x_id = sbxid;

    sbptr->geo.x =  sx;
    sbptr->geo.y =  sy;
    sbptr->geo.dx = sdx;
    sbptr->geo.dy = sdy;
    sbptr->geo.bw = 0;

    sbptr->dir = dir;

    sbptr->butstart = 0;
    sbptr->butend   = sdy/10;
/*
***Return ptr to slidebar.
*/
   *outptr = sbptr;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        void    WPexpose_slidebar(
        WPSBAR *sbptr)

/*      Expose handler for WPSBAR.
 *
 *      In: buttptr = C ptr to WPSBAR.
 *
 *      (C)2007-10-18J.Kjellander
 *
 ******************************************************!*/

  {
    WPWIN  *winptr;
    WPLWIN *lwinpt;
    int     pdx,pdy,x1,y1,x2,y2;
    Window  pxid;

/*
***Clear window.
*/
   XClearWindow(xdisp,sbptr->id.x_id);
/*
***Parent size etc.
*/
   winptr=WPwgwp(sbptr->id.p_id);

   if ( winptr->typ == TYP_LWIN )
     {
     lwinpt = (WPLWIN *)winptr->ptr;
     pxid   = lwinpt->id.x_id;
     pdx    = lwinpt->geo.dx;
     pdy    = lwinpt->geo.dy;
     }
   else return;
/*
***Horizontal or vertical ?
*/
    switch ( sbptr->dir )
      {
/*
***Vertical.
*/
      case WP_SBARV:
      x1 = x2 = 0;
      y1 = 0;
      y2 = sbptr->geo.dy;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 0;
      x2 = sbptr->geo.dx;
      y1 = y2 = sbptr->butstart;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 0;
      x2 = sbptr->geo.dx;
      y1 = y2 = sbptr->butend;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 1;
      x2 = sbptr->geo.dx;
      y1 = sbptr->butstart + 1;
      y2 = sbptr->butend;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      XFillRectangle(xdisp,sbptr->id.x_id,xgc,x1,y1,x2-x1,y2-y1);
      break;
      }
/*
***Flush.
*/
   XFlush(xdisp);
/*
***The end.
*/
    return;
  }

/********************************************************/
/********************************************************/

        bool          WPbutton_slidebar(
        WPSBAR       *sbptr,
        XButtonEvent *butev)

/*      Button handler for WPSBAR. Handles scroll.
 *
 *      In: sbptr = C ptr to WPSBAR.
 *          butev = X1 Button event.
 *
 *      (C)2007-10-18 J. Kjellander
 *
 ******************************************************!*/

  {
   int     butsiz,mouse_y;
   XEvent  event;
   WPWIN  *winptr;
   WPLWIN *lwinpt;

/*
***Check that left mouse button was used.
*/
   if ( butev->button != 1 ) return(FALSE);
/*
***Ptr to parent WPLWIN.
*/
   winptr = WPwgwp(sbptr->id.p_id);
   lwinpt = (WPLWIN *)winptr->ptr;
/*
***Button size.
*/
   butsiz = sbptr->butend - sbptr->butstart;
/*
***Start of event loop.
*/
loop:
   XNextEvent(xdisp,&event);

   switch ( event.type )
     {
/*
***Mouse move.
*/
     case MotionNotify:
     mouse_y = event.xmotion.y;
     while ( XPending(xdisp) )
       {
       XNextEvent(xdisp,&event);
       if ( event.type == MotionNotify )
         {
         mouse_y = event.xmotion.y;
         }
       }
     if       ( mouse_y < 1 )                          mouse_y = 0;
     else if ( mouse_y > sbptr->geo.dy - butsiz - 1 ) mouse_y = sbptr->geo.dy - butsiz - 1;
     sbptr->butstart = mouse_y;
     sbptr->butend = mouse_y + butsiz;
     WPxplw(lwinpt);
     goto loop;
/*
***Button is released.
*/
     case ButtonRelease:
     while ( XPending(xdisp) ) XNextEvent(xdisp,&event);
     sbptr->butstart = mouse_y;
     sbptr->butend = mouse_y + butsiz;
     WPxplw(lwinpt);
     return(TRUE);
/*
***Expose.
*/
     case Expose:
     WPexpose_slidebar(sbptr);
     goto loop;
/*
***Let's just skip all other events.
*/
     default:
     goto loop;
     }
  }

/********************************************************/
