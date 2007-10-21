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
   int                  pdx,pdy,sx,sy,sdx,sdy,wintype;
   XSetWindowAttributes xwina;
   unsigned long        xwinm;
   WPSBAR              *sbptr;

/*
***Get a C pointer to the parent's entry in wpwtab.
*/
   if ( (winptr=WPwgwp(pid)) == NULL ) return(-3);
/*
***Parent geometry etc.
*/
   wintype = winptr->typ;

   switch ( wintype )
     {
     case TYP_LWIN:
     lwinpt = (WPLWIN *)winptr->ptr;
     pxid   = lwinpt->id.x_id;
     pdx    = lwinpt->geo.dx;
     pdy    = lwinpt->geo.dy;
     break;

     default:
     return(-3);
     }
/*
***Slidebar window position and size relative
***to parent window.
*/
   switch ( dir )
     {
     case WP_SBARV:
     sx  = pdx - WPstrh();
     sy  = 0;
     sdx = WPstrh();
     sdy = pdy - WPstrh();
     break;

     case WP_SBARH:
     sx  = 0;
     sy  = pdy - WPstrh();
     sdx = pdx - WPstrh();
     sdy = WPstrh();
     break;
     }
/*
***Create the X window.
*/
   xwina.background_pixel  = WPgcbu(WPwfpx(pxid),0);
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
/*
***Slidebar button size.
*/
   switch ( wintype )
     {
     case TYP_LWIN:
     sbptr->butstart = 0;
     if ( sbptr->dir == WP_SBARV )
       {
       sbptr->butend   = (double)((double)lwinpt->nl_vis/(double)lwinpt->nl_tot)*sdy;
       if ( (sbptr->butend - sbptr->butstart) < 15 ) sbptr->butend = 15;
       }
     else
       {
       sbptr->butend = ((double)(sdx - 2*WPstrh())/(double)lwinpt->maxlen)*
                                (sdx - 2*WPstrh());
       if ( (sbptr->butend - sbptr->butstart) < 15 ) sbptr->butend = 15;
       }
     break;
     }
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
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND1));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 2;
      x2 = sbptr->geo.dx;
      y1 = y2 = sbptr->butend - 1;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 0;
      x2 = sbptr->geo.dx;
      y1 = y2 = sbptr->geo.dy - 1;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND1));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 2;
      x2 = sbptr->geo.dx;
      y1 = sbptr->butstart + 2;
      y2 = sbptr->butend   - 1;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND2));
      XFillRectangle(xdisp,sbptr->id.x_id,xgc,x1,y1,x2-x1,y2-y1);

      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      x1 = 3;
      x2 = sbptr->geo.dx - 3;
      y1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2 - (x2 - x1)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y1);
      y1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y1);
      y1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2 + (x2 - x1)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y1);
      break;
/*
***Horizontal.
*/
      case WP_SBARH:
      x1 = 0;
      x2 = sbptr->geo.dx;
      y1 = y2 = 0;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND1));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = x2 = sbptr->butend - 1;
      y1 = 2;
      y2 = sbptr->geo.dy;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = x2 = sbptr->geo.dx - 1;
      y1 = 0;
      y2 = sbptr->geo.dy;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND1));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = sbptr->butstart + 2;
      x2 = sbptr->butend   - 1;
      y1 = 2;
      y2 = sbptr->geo.dy;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND2));
      XFillRectangle(xdisp,sbptr->id.x_id,xgc,x1,y1,x2-x1,y2-y1);

      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      y1 = 3;
      y2 = sbptr->geo.dy - 3;
      x1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2 - (y2 - y1)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x1,y2);
      x1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x1,y2);
      x1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2 + (y2 - y1)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x1,y2);
      break;
      }
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
 *          butev = X11 Button event.
 *
 *      (C)2007-10-18 J. Kjellander
 *
 ******************************************************!*/

  {
   int     butsize,mouse_pos,mouse_offset,sblen,wintype;
   XEvent  event;
   WPWIN  *winptr;
   WPLWIN *lwinpt;

/*
***Check that it was a ButtonPress.
*/
   if ( butev->type != ButtonPress ) return(FALSE);
/*
***Check that left mouse button was used.
*/
   if ( butev->button != 1 ) return(FALSE);
/*
***Check that the mouse was inside the button.
*/
   if ( sbptr->dir == WP_SBARV ) mouse_pos = butev->y;
   else                          mouse_pos = butev->x;

   if ( mouse_pos  < sbptr->butstart  ||
        mouse_pos  > sbptr->butend ) return(FALSE);
/*
***Grab the pointer.
*/
   XGrabPointer(xdisp,sbptr->id.x_id,FALSE,ButtonReleaseMask | ButtonMotionMask,GrabModeAsync,
                GrabModeAsync,None,None,CurrentTime);
/*
***Slidebar length.
*/
   if ( sbptr->dir == WP_SBARV ) sblen = sbptr->geo.dy;
   else                          sblen = sbptr->geo.dx;
/*
***Button size.
*/
   butsize = sbptr->butend - sbptr->butstart;
/*
***Offset between the mouse position
***and the start of the button (integer > 0).
*/
   mouse_offset = mouse_pos - sbptr->butstart;
/*
***Ptr to parent window.
*/
   winptr = WPwgwp(sbptr->id.p_id);
   wintype = winptr->typ;

   switch ( wintype )
     {
     case TYP_LWIN:
     lwinpt = (WPLWIN *)winptr->ptr;
     break;
/*
     default:
     return(FALSE);
*/
     }
/*
***Start of event loop.
*/
loop:
   XNextEvent(xdisp,&event);

   switch ( event.type )
     {
/*
***Mouse move. Check that mouse is moving within slidebar
***limits, update the button position and the parent
***window.
*/
     case MotionNotify:
     if ( sbptr->dir == WP_SBARV ) mouse_pos = event.xmotion.y;
     else                          mouse_pos = event.xmotion.x;

/*
***Remove pending motion events.
*/
     while ( XCheckMaskEvent(xdisp,PointerMotionMask,&event) )
       {
       if ( sbptr->dir == WP_SBARV ) mouse_pos = event.xmotion.y;
       else                          mouse_pos = event.xmotion.x;
       }


/*
     while ( XPending(xdisp) )
       {
       XNextEvent(xdisp,&event);
       if ( event.type == MotionNotify )
         {
         if ( sbptr->dir == WP_SBARV ) mouse_pos = event.xmotion.y;
         else                          mouse_pos = event.xmotion.x;
         }
       }
*/

     if ( mouse_pos < mouse_offset )
       mouse_pos = mouse_offset;
     else if ( mouse_pos > sblen - butsize + mouse_offset )
       mouse_pos = sblen - butsize + mouse_offset;
     sbptr->butstart = mouse_pos - mouse_offset;
     sbptr->butend   = sbptr->butstart + butsize;

     switch ( wintype )
       {
       case TYP_LWIN:
       WPxplw(lwinpt);
       break;
       }

     goto loop;
/*
***Button is released. Update parent window and exit.
*/
     case ButtonRelease:
     XUngrabPointer(xdisp,CurrentTime);
     while ( XPending(xdisp) ) XNextEvent(xdisp,&event);

     switch ( wintype )
       {
       case TYP_LWIN:
       WPxplw(lwinpt);
       break;
       }

     return(TRUE);
/*
***Let's just skip all other events.
*/
     default:
     goto loop;
     }
  }

/********************************************************/
