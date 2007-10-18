/**********************************************************************
*
*    wpLWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPinla();     Init list window (WPLWIN)
*    WPalla();     Add lines to WPLWIN
*    WPexla();     Exit list window
*
*    WPxplw();     Expose routine for WPLWIN
*    WPbtlw();     Button routine for WPLWIN
*    WPcrlw();     Crossing routine for WPLWIN
*    WPcmlw();     ClientMessage routine for WPLWIN
*    WPdllw();     Delete WPLWIN
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
#include "../../EX/include/EX.h"
#include "../include/WP.h"
#include <string.h>

extern char  jobdir[],jobnam[];
extern char *mktemp();

static WPLWIN  *actlwin = NULL;

/*
***actlwin is a C ptr to a WPLWIN which has been created (WPinla)
***but not yet closed (WPexla). If actlwin = NULL no WPLWIN is
***open but there may exist many closed WPLWIN.
*/


static char title [81];        /* varkon.list.title  */

/*
***Internal functions.
*/
static short crlwin(int,int,int,int,char *wtitle);
static short savelw(WPLWIN *lwinpt);

/*!******************************************************/

        short WPinla(char *hs)

/*      Create (open) a WPLWIN. LST_INI(title) in MBS.
 *
 *      In: hs = Header string (title).
 *
 *      (C)microform ab 22/7/92 U. Andersson
 *
 *      7/12/93  Omarbetad, J. Kjellander
 *      8/11/94  Resurser f�r texterna, J. Kjellander
 *      1996-04-25 6 st. "X", J. Kjellander
 *      1997-01-15 IGgenv(), J.Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    char     templ[V3PTHLEN+1],tmpfil[V3PTHLEN+1],
             tmpbuf[V3STRLEN+1];
    wpw_id   id;
    int      dx;
    WPLWIN  *lwinptr;
    FILE    *fp;

/*
***Check that another WPLWIN is not already opened.
*/
    if ( actlwin != NULL )
      {
      WPexla(TRUE);
      return(-2);
      }
/*
***Create a temporary file for list contents.
*/
    strcpy(templ,IGgenv(VARKON_TMP));
    strcat(templ,jobnam);
    strcat(templ,".XXXXXX");
    mktemp(templ);
    strcpy(tmpfil,templ);

    if ( (fp=fopen(tmpfil,"w+")) == NULL ) return(-2);
/*
***Get text resources from the ini-file.
*/
    if ( !WPgrst("varkon.list.title",title) ) strcpy(title," ");
    if (  WPgrst("varkon.list.title.jobnam",tmpbuf)  &&
          strcmp(tmpbuf,"True") == 0 ) strcat(title,jobnam);
/*
***Allocate a window-ID.
*/
    if ( (id=WPwffi()) < 0 ) return(-2);
/*
***Create an opened WPLWIN. Note that there is still
***no X window created. This is done when the WPLWIN
***is closed.
*/
    if ( (lwinptr=(WPLWIN *)v3mall(sizeof(WPLWIN),"WPinla"))
                                                   == NULL ) return(-2);

    lwinptr->id.w_id = id;
    lwinptr->id.p_id = 0;
    lwinptr->id.x_id = 0;

    lwinptr->geo.x =  0;
    lwinptr->geo.y =  0;
    lwinptr->geo.dx =  0;
    lwinptr->geo.dy =  0;

    lwinptr->psbar_h = NULL;
    lwinptr->psbar_v = NULL;

    lwinptr->filpek = fp;
    strcpy(lwinptr->filnam,tmpfil);

    strcpy(lwinptr->rubrik,hs);
/*
***Calculate minimum window width.
*/
    dx = WPstrl(title);

    if ( WPstrl(hs) > dx ) dx = WPstrl(hs);

    lwinptr->maxrln = dx;

    lwinptr->nl_first = 0;    /* First visible line */
    lwinptr->nl_tot   = 0;    /* Total number of lines */
    lwinptr->nl_vis   = 0;    /* Number of lines in window */
/*
***Enter opened WPLWIN in global window table.
*/
    wpwtab[id].typ = TYP_LWIN;
    wpwtab[id].ptr = (char *)lwinptr;
/*
***This WPLWIN is now opened but not closed.
*/
    actlwin = lwinptr;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPalla(
        char *s,
        short rs)

/*      Add a line to the currently open WPLWIN.
 *
 *      In: s  = Text string.
 *          rs = Line space.
 *
 *      (C)microform ab 22/7/92 U. Andersson 
 *
 *      931207 Omarbetad, J. Kjellander
 *      1998-03-11 L�ngre rader, J.Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    int lt,i,rest,nstkn;
    char mell[V3STRLEN+1],rad[V3STRLEN+1];

/*
***An open WPLWIN must exist.
*/
    if ( actlwin == NULL ) return(-2);
/*
***Check line length. Truncate at V3STRLEN.
*/
    nstkn = strlen(s); 
    if ( nstkn > V3STRLEN )
      {
      s[V3STRLEN] = '\0';
      nstkn = V3STRLEN;
      }
/*
***Line length in pixels.
*/
    lt = WPstrl(s);
    if ( lt > actlwin->maxrln ) actlwin->maxrln = lt;
/*
***If line is shorter than V3STRLEN, pad with space.
*/
    rest  = V3STRLEN - nstkn; 
    for ( i=0; i<rest; i++ ) mell[i] = ' ';
    mell[rest] = '\0';
    strcpy(rad,s);
    strcat(rad,mell);
    fprintf(actlwin->filpek,"%s\n",rad);
/*
***Update line counter.
*/
  ++actlwin->nl_tot;
/*
***Optional line spacing.
*/
    if ( rs > 1 )
      {
      for (i=0;i<V3STRLEN;i++) mell[i] = ' ';
      mell[V3STRLEN] = '\0';

      for ( i=0; i<rs - 1; i++ )
        {
        ++actlwin->nl_tot;
        fprintf(actlwin->filpek,"%s\n",mell);
        }
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPexla(
        bool  show)

/*      Close an open WPLWIN.
 *
 *      In: show = TRUE  => Display.
 *                 FALSE => Don't display.
 *
 *      (C)microform ab 22/7/92 U. Andersson 
 *
 *       8/12/93    Omarbetad, J. Kjellander
 *       7/11/94    Resurser f�r placering, J. Kjellander
 *       1996-02-05 show, J. Kjellander
 *       2007-20-28 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      x,y,maxnl,nl,a,b,dx,dy;
    unsigned int dum1,dum2;
    double   c,d;
    char    *type[20];
    XrmValue value;
    WPWIN   *winptr;
    WPSBAR  *sbptr;

/*
***A WPLWIN must be open.
*/
   if ( actlwin == NULL ) return(-2);
/*
***Close temporary file.
*/
   fclose(actlwin->filpek);
/*
***If no show it's simple. Remove temp. file, and
***kill open WPLWIN created by WPinla().
*/
   if ( !show)
     {
     IGfdel(actlwin->filnam);
     if ( (winptr=WPwgwp(actlwin->id.w_id)) != NULL )
       {
       winptr->typ = TYP_UNDEF;
       winptr->ptr = NULL;
       v3free((char *)actlwin,"WPexla");
       }
     actlwin = NULL;
     return(0);
     }
/*
***WPLWIN height.
*/
    maxnl = (int)(0.6*DisplayHeight(xdisp,xscr)/WPstrh()); /* Max number of lines */
    nl = actlwin->nl_tot;

    if ( nl > maxnl ) nl = maxnl;

    dy = WPstrh()*(nl + 4);
/*
***WPLWIN width.
*/
    dx = WPstrh() + actlwin->maxrln + WPstrh();
/*
***WPLWIN position.
*/
    x  = 90;
    y  = DisplayHeight(xdisp,xscr) - dy - 50; 

    if ( XrmGetResource(xresDB,"varkon.list.geometry","Varkon.List.Geometry",
         type,&value) ) XParseGeometry((char *)value.addr,&x,&y,&dum1,&dum2);
/*
***Create the window in X.
*/
    crlwin(x,y,dx,dy,title);
/*
***What size did it get (after possible fight with a WM)?
*/
    WPgwsz(actlwin->id.x_id,&a,&b,&dx,&dy,&c,&d);

    actlwin->geo.dx = dx;
    actlwin->geo.dy = dy;
/*
***How many lines visible.
*/
    actlwin->nl_vis = dy/WPstrh() - 4;
/*
***Add optional slidebar(s).
*/
    status = WPcreate_slidebar(actlwin->id.w_id,WP_SBARV,&sbptr);
    actlwin->psbar_v = sbptr;
    sbptr->id.p_id = actlwin->id.w_id;
/*
***Display.
*/
    WPwshw(actlwin->id.w_id);
/*
***The open WPLWIN is now closed.
*/
    actlwin = NULL;
/*
***The end.
*/
    return(0);
  }
/********************************************************/
/*!******************************************************/

        bool    WPxplw(
        WPLWIN *lwinpt)

/*      Expose handler for WPLWIN.
 *
 *      In: lwinpt = C-pekare till WPLWIN.
 *
 *      (C)microform ab 11/7/92 U. Andersson.
 *
 *      7/12/93  Omarbetad, J. Kjellander
 *      1998-03-11 L�ngre rader, J.Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    char    rad[V3STRLEN+3];
    int     j,tx,ty,a,b,px,py,butsiz;
    double  c,d;

/*
***Colors.
*/
    XSetBackground(xdisp,xgc,WPgcol(0));
    XSetForeground(xdisp,xgc,WPgcol(1));
/*
***Current WPLWIN size.
*/
    WPgwsz(lwinpt->id.x_id,&a,&b,&px,&py,&c,&d);
/*
***Optional slidebar status.
*/
    if ( (lwinpt->nl_tot > lwinpt->nl_vis) && (lwinpt->psbar_v != NULL) )
      {
      butsiz = lwinpt->psbar_v->butend - lwinpt->psbar_v->butstart;
      if ( lwinpt->psbar_v->butstart == 0 ) lwinpt->nl_first = 0;
      else                                 lwinpt->nl_first = ((double)lwinpt->psbar_v->butstart/
                                            ((double)(lwinpt->psbar_v->geo.dy - butsiz))*
                                            (lwinpt->nl_tot - lwinpt->nl_vis)) + 1;
      if ( lwinpt->nl_first > lwinpt->nl_tot - lwinpt->nl_vis )
        lwinpt->nl_first = lwinpt->nl_tot - lwinpt->nl_vis - 1;
      }
/*
***How many lines are visible ?
*/
    lwinpt->nl_vis = py/WPstrh() - 4;
/*
***Current position in pixels.
*/
    tx = ty = 2*WPstrh();
/*
***Display header string.
*/
    WPwstr(lwinpt->id.x_id,tx,ty,lwinpt->rubrik);
/*
***Empty line.
*/
    ty = ty + WPstrh();
/*
***Open list file.
*/
    lwinpt->filpek = fopen(lwinpt->filnam,"r");
/*
***Position to first visible line.
*/
    fseek(lwinpt->filpek,lwinpt->nl_first*(V3STRLEN+1),SEEK_SET);
/*
***Read lines and display.
*/
    for ( j=0; j<lwinpt->nl_vis; ++j )
      {
      if ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL )
        {
        ty +=  WPstrh();
        rad[V3STRLEN] = '\0';
        WPwstr(lwinpt->id.x_id,tx,ty,rad);
        }
      else break;
      }
/*
***Close the list file.
*/
    fclose(lwinpt->filpek); 
/*
***Expose optional slidebars.
*/
   if ( lwinpt->psbar_h != NULL ) WPexpose_slidebar(lwinpt->psbar_h);
   if ( lwinpt->psbar_v != NULL ) WPexpose_slidebar(lwinpt->psbar_v);
/*
***The end.
*/
    return(0);
 }

/*********************************************************/
/*!******************************************************/

        bool          WPbtlw(
        WPLWIN       *lwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button handler for WPLWIN.
 *
 *      In: iwinpt    = C ptr to WPLWIN.
 *          butev     = C ptr to X-event.
 *
 *      Out: *serv_id = ID of subwindow that served the event.
 *
 *      Out: TRUE =  Event served.
 *           FALSE = This window (with subwindows) not involved.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1997-01-16 Bug, butptr=NULL, J.Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Mouse event in the actual WPLWIN ?
*/
    if ( butev->window == lwinpt->id.x_id )
      {
     *serv_id = lwinpt->id.w_id;

      switch ( butev->button )
        {
/*
***TODO
*/
        case 1:
        break;
/*
***
*/
        case 2:
        break;
/*
***
*/
        case 3:
        break;
/*
***Something else should not be possible.
*/
        default:
        return(FALSE);
        }
      return(TRUE);
      }
/*
***Optional slidebars.
*/
    if ( lwinpt->psbar_v != NULL  &&  butev->window == lwinpt->psbar_v->id.x_id )
      {
      WPbutton_slidebar(lwinpt->psbar_v,butev);
     *serv_id = lwinpt->id.w_id;
      return(TRUE);
      }
/*
***The end.
*/
    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool            WPcrlw(
        WPLWIN         *lwinpt,
        XCrossingEvent *croev)

/*      Crossing handler for WPLWIN with sub windows.
 *
 *      In: lwinpt = C ptr to WPLWIN.
 *
 *      Return: TRUE  => Event served.
 *              FALSE => Not this window.
 *
 *      (C)2007-10-18 J. Kjellander
 *
 ******************************************************!*/

  {
    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool                  WPcmlw(
        WPLWIN               *lwinpt,
        XClientMessageEvent  *clmev)

/*      ClientMessage handler for WPLWIN.
 *
 *      In: iwinpt  = C ptr to WPLWIN.
 *          clmev   = C ptr to event.
 *
 *      FV: TRUE  = Eventet served.
 *          FALSE = Not this window.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
*** If it's a WM_DELETE_WINDOW, delete the window.
*/
   if ( clmev->message_type ==
        XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]    ==
        XInternAtom(xdisp,"WM_DELETE_WINDOW",False) )
     {
     WPwdel((DBint)lwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short   WPdllw(
        WPLWIN *lwinpt)

/*      Kill a WPLWIN window.
 *
 *      In: lwinpt = C-ptr to WPLWIN.
 *
 *      (C)microform ab 24/7/92 U. Andersson 
 *
 *      7/12/93 Omarbetad, J. Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

   {
/*
***Remove temporary file.
*/
    IGfdel(lwinpt->filnam);
/*
***Deallocate memory for subwindows.
*/
    if ( lwinpt->psbar_h != NULL ) v3free((char *)lwinpt->psbar_h,"WPdllw");
    if ( lwinpt->psbar_v != NULL ) v3free((char *)lwinpt->psbar_v,"WPdllw");
/*
***Free memory for the WPLWIN itself.
*/
    v3free((char *)lwinpt,"WPdllw");
/*
***Slut.
*/
    return(0);
   }
/********************************************************/
/*!******************************************************/

 static short crlwin(
        int   x,
        int   y,
        int   dx,
        int   dy,
        char *wtitle)

/*      Creates a WPLWIN in X
 *
 *      In: x,y    = Window position.
 *          dx,dy  = Window size.
 *          wtitel = Window title.
 *
 *      (C)microform ab 19/7/92 U. Andersson
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *      1998-03-11 L�ngre rader, J.Kjellander
 *      2006-12-19 ButtonReleaseMask, J.Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    XSizeHints           xhint;
    char                 titel[V3STRLEN];

/*
***Title.
*/
    strcpy(titel,"l-");
    strcat(titel,jobnam);
/*
***Attributes.
*/
    xwina.background_pixel = WPgcol(0);
    xwina.border_pixel = BlackPixel( xdisp, xscr );
    xwina.override_redirect = False;
    xwina.save_under = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );
/*
***Create window.
*/
    actlwin->id.x_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),
                                     x,y,dx,dy,3,
                                     DefaultDepth(xdisp,xscr),
                                     InputOutput,
                                     CopyFromParent,xwinm,&xwina);

    xhint.flags  = USPosition | USSize | PMinSize | PMaxSize;
    xhint.x = x;
    xhint.y = y;
    xhint.width  = dx;
    xhint.height = dy;
    xhint.min_height = 4*WPstrh();
    xhint.min_width  = WPstrl(wtitle);
    xhint.max_width  = V3STRLEN*WPstrl("w")+2*WPstrh();
    xhint.max_height = (int)(0.8*DisplayHeight(xdisp,xscr));

    XSetNormalHints(xdisp,actlwin->id.x_id,&xhint);

    XStoreName(xdisp,actlwin->id.x_id,wtitle);
    XSetIconName(xdisp,actlwin->id.x_id,titel);
/*
***Set the WM delete protocol.
*/
    WPsdpr(actlwin->id.x_id);
/*
***Event mask.
*/
    XSelectInput(xdisp,actlwin->id.x_id,ExposureMask    |
                                        ButtonPressMask |
                                        ButtonReleaseMask);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short savelw(
        WPLWIN *lwinpt)

/*      Sparar det angivna listf�nstret eller
 *      hela listfilen p� fil eller utskrift till valfri skrivare.
 *
 *      In: lwinpt = C-pekare till list-f�nster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 7/8/92 U. Andersson 
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *      1997-01-25 printer, J.Kjellander
 *      1998-03-11 L�ngre rader, J.Kjellander
 *      2004-09-03 English texts, Johan Kjellander, �rebro university
 *
 ******************************************************!*/

  {
   int   radb,r = 0;
   char *slt = "Entire list or window";
   char *est = "e";
   char *wst = "w";  
   char *spt = "  File or printer  ";  
   char *fst = "f";
   char *pst = "p";  
   char  rad[V3STRLEN+2];
   bool  hela,fil;
   short status;
   char  oscmd[V3PTHLEN+25],fnam[V3PTHLEN],printer[V3STRLEN];
   FILE *tempfil;

   static char dstr[V3PTHLEN] = "";

/*
***Anrop till alternativ funktionen WPialt
***ska vi spara hela listan eller bara f�nstret?. 
*/
    hela = WPialt(slt,est,wst,FALSE);
/*
***Spara aktiv radb�rjan.
*/
    radb = lwinpt->nl_first;
/*
***Ska vi spara hela listan?. 
*/
    if ( hela == TRUE )  lwinpt->nl_first = 0;
/*
***Anrop till alternativ funktionen WPialt
***spara p� fil eller skrivare. 
*/
    fil = WPialt(spt,fst,pst,FALSE);
/*
***Vad skall filen heta ?
*/
    if ( dstr[0] == '\0' )
      {
      strcpy(dstr,jobdir);
      strcat(dstr,jobnam);
      strcat(dstr,LSTEXT);
      }

    if ( fil == TRUE )
      {
      status = IGssip("","Enter filename :",fnam,dstr,V3PTHLEN);
      if ( status < 0 ) goto end;
      strcpy(dstr,fnam);
      }
    else
      {
      strcpy(fnam,dstr);
      }
/*
***�ppna listfil f�r l�sning.
*/
    lwinpt->filpek = fopen(lwinpt->filnam,"r");
/*
***�ppna  ny fil f�r skrivning.
*/
    tempfil = fopen(fnam,"w+");
/*
***Skriv in �verskrift och en tomrad f�rst i tmpfil.
*/
    fprintf(tempfil,"%s\n\n",lwinpt->rubrik);
/*
***Spara listfilen p� en valfri fil.
***Vilken rad ska vi b�rja l�sningen ifr�n.
*/
     fseek(lwinpt->filpek,lwinpt->nl_first*81,SEEK_SET);

     if ( hela == TRUE ) 
       {
       while ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL) 
         {
         rad[V3STRLEN] = '\0';
         fprintf(tempfil,"%s\n",rad);
         }
       }
     else 
       {
       while ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL &&
               r < lwinpt->nl_vis ) 
         {
         rad[V3STRLEN] = '\0';
         fprintf(tempfil,"%s\n",rad);
         ++r;
         }
       }
/*
***St�ng filerna.
*/
    fclose(lwinpt->filpek);
    fclose(tempfil);
/*
***Tilldela radb�rjan sitt ursprungliga v�rde.
*/
    lwinpt->nl_first = radb;
/*
***Ev. utskrift p� skrivare.
*/
    if ( !WPgrst("varkon.list.printer",printer) ) strcpy(printer,"lp");

    if ( fil == FALSE )
      {
      strcpy(oscmd,"cat ");
      strcat(oscmd,fnam);
      strcat(oscmd," | ");
      strcat(oscmd,printer);
      EXos(oscmd,2);
      }
/*
***Slut.
*/
end:
    return(0);
  }
/********************************************************/
