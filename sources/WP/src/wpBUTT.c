/**********************************************************************
*
*    wpBUTT.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPcrfb();      Create WPBUTT, CRE_FBUTTON in MBS
*    WPmcbu();      Create WPBUTT, CRE_BUTTON in MBS
*    WPwcbu();      Create WPBUTT, wpw-version
*    WPxpbu();      Expose routine for WPBUTT
*    WPscbu();      Set button color
*    WPbtbu();      Button routine for WPBUTT
*    WPcrbu();      Crossing routine for WPBUT
*    WPgtbu();      Get routine for WPBUTT, GET_BUTTON in MBS
*    WPdlbu();      Kills WPBUTT
*    WPgcbu();      Returns button color
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

        short WPcrfb(
        int    pid,
        short  x,
        short  y,
        short  dx,
        short  dy,
        char  *butstr,
        char  *akod,
        short  anum,
        DBint *bid)

/*      Create button in graphical window.
 *
 *      In: pid    = ID fï¿½r grafiskt fï¿½nster.
 *          x,y    = Placering.
 *          dx,dy  = Storlek.
 *          butstr = Knapptext.
 *          akod   = Aktionskod.
 *          anum   = Aktionsnummer.
 *          bid    = Pekare till resultat.
 *
 *      Ut: *bid = Button ID.
 *
 *      Felkod: 
 *              WP1512 = %s ï¿½r en otillï¿½ten aktionskod.
 *              WP1482 = Fï¿½nstret %s finns ej
 *              WP1492 = Fï¿½nstret %s ï¿½r av fel typ
 *              WP1502 = Fï¿½nster %s ï¿½r fullt
 *
 *      (C)microform ab 1996-05-20 J. Kjellander
 *
 ******************************************************!*/

  {
    short    status,action;
    int      i;
    char     errbuf[80];
    WPWIN   *winptr;
    WPGWIN  *gwinpt;
    WPBUTT  *butptr;

/*
***Vilken aktionskod ?
*/
    if ( akod[1] != '\0' ) return(erpush("WP1512",akod));

    switch ( akod[0] )
      {
      case 'f': action = CFUNC; break;
      case 'm': action = MENU;  break;
      case 'p': action = PART;  break;
      case 'r': action = RUN;   break;
      case 'M': action = MFUNC; break;
  
      default: return(erpush("WP1512",akod));
      break;
      }
/*
***Fixa C-pekare till det grafiska fï¿½nstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1482",errbuf));
      }
/*
***Kolla att det ï¿½r ett WPGWIN och fixa en pekare till
***fï¿½rï¿½lder-fï¿½nstret sjï¿½lvt.
*/
    if ( winptr->typ != TYP_GWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1492",errbuf));
      }
    else gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skapa ID fï¿½r den nya knappen, dvs fixa
***en ledig plats i fï¿½rï¿½lderns fï¿½nstertabell.
*/
    i = 0;
    while ( i < WP_GWSMAX  &&  gwinpt->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_GWSMAX )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1502",errbuf));
      }
    else *bid = i;
/*
***Prova att skapa en tryckknapp.
*/
    status = WPwcbu(gwinpt->id.x_id,x,y,dx,dy,(short)1,
                        butstr,butstr,"",WP_BGND2,WP_FGND,&butptr);

    if ( status < 0 ) return(status);
/*
***Lï¿½nka in den i WPGWIN-fï¿½nstret.
*/
    gwinpt->wintab[*bid].typ = TYP_BUTTON;
    gwinpt->wintab[*bid].ptr = (char *)butptr;

    butptr->id.w_id = *bid;
    butptr->id.p_id =  pid;

    XMapWindow(xdisp,butptr->id.x_id);
/*
***Aktion.
*/
    butptr->acttyp = action;
    butptr->actnum = anum;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPmcbu(
        wpw_id  pid,
        short   x,
        short   y,
        short   dx,
        short   dy,
        short   bw,
        char   *str1,
        char   *str2,
        char   *fstr,
        short   cb,
        short   cf,
        DBint  *bid)

/*      Skapar WPBUTT-fï¿½nster och lï¿½nkar in i ett WPIWIN.
 *      CRE_BUTTON i MBS.
 *
 *      In: pid   = Fï¿½rï¿½lder.
 *          x     = Lï¿½ge i X-led.
 *          y     = Lï¿½ge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          bw    = Border-width.
 *          str1  = Text i lï¿½ge off/FALSE.
 *          str2  = Text i lï¿½ge on/TRUE.
 *          fstr  = Fontnamn eller "" (default).
 *          cb    = Bakgrundsfï¿½rg.
 *          cf    = Fï¿½rgrundsfï¿½rg.
 *          bid   = Pekare till utdata.
 *
 *      Ut: *bid = Giltigt entry i fï¿½rï¿½lderns wintab.
 *
 *      Felkod: WP1072 = Fï¿½rï¿½ldern %s finns ej.
 *              WP1082 = Fï¿½rï¿½ldern %s ï¿½r ej ett WPIWIN.
 *              WP1092 = Fï¿½r mï¿½nga subfï¿½nster i %s.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char                 errbuf[80];
    short                i,status;
    WPWIN               *winptr;
    WPIWIN              *iwinptr;
    WPBUTT              *butptr;

/*
***Fixa C-pekare till fï¿½rï¿½lderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1072",errbuf));
      }
/*
***Kolla att det ï¿½r ett WPIWIN och fixa en pekare till
***fï¿½rï¿½lder-fï¿½nstret sjï¿½lvt.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1082",errbuf));
      }
    else iwinptr = (WPIWIN *)winptr->ptr;
/*
***Skapa ID fï¿½r den nya knappen, dvs fixa
***en ledig plats i fï¿½rï¿½lderns fï¿½nstertabell.
*/
    i = 0;
    while ( i < WP_IWSMAX  &&  iwinptr->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_IWSMAX )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1092",errbuf));
      }
    else *bid = i;
/*
***Skapa knappen.
*/
    if ( (status=WPwcbu(iwinptr->id.x_id,x,y,dx,dy,bw,
                        str1,str2,fstr,cb,cf,&butptr)) < 0 ) return(status);
/*
***Lï¿½nka in den i WPIWIN-fï¿½nstret.
*/
    iwinptr->wintab[*bid].typ = TYP_BUTTON;
    iwinptr->wintab[*bid].ptr = (char *)butptr;

    butptr->id.w_id = *bid;
    butptr->id.p_id =  pid;
/*
***Om WPIWIN-fï¿½nstret redan ï¿½r mappat skall knappen mappas nu.
*/
    if ( iwinptr->mapped ) XMapWindow(xdisp,butptr->id.x_id);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    WPwcbu(
        Window   px_id,
        short    x,
        short    y,
        short    dx,
        short    dy,
        short    bw,
        char    *str1,
        char    *str2,
        char    *fstr,
        short    cb,
        short    cf,
        WPBUTT **outptr)

/*      Skapar WPBUTT-fï¿½nster.
 *
 *      In: px_id  = Fï¿½rï¿½ldra fï¿½nstrets X-id.
 *          x      = Lï¿½ge i X-led.
 *          y      = Lï¿½ge i Y-led.   
 *          dx     = Storlek i X-led.
 *          dy     = Storlek i Y-led.
 *          bw     = Border-width.
 *          str1   = Text i lï¿½ge off/FALSE.
 *          str2   = Text i lï¿½ge on/TRUE.
 *          fstr   = Fontnamn eller "".
 *          cb     = Bakgrundsfï¿½rg.
 *          cf     = Fï¿½rgrundsfï¿½rg.
 *          outptr = Pekare till utdata.
 *
 *      Ut: *outptr = Pekare till WPBUTT.
 *
 *      Felkod: WP1102 = Fonten %s finns ej.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2006-12-11 Added ButtonReleaseMask, J.Kjellander
 *      2007-03-08 Tooltips, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    Window               xwin_id;
    WPBUTT              *butptr;

/*
***Skapa fï¿½nstret i X.
*/
    xwina.background_pixel  = WPgcbu(WPwfpx(px_id),cb);
    xwina.border_pixel      = WPgcbu(WPwfpx(px_id),WP_BGND1);
    xwina.override_redirect = True;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  

    xwin_id = XCreateWindow(xdisp,px_id,x,y,dx,dy,bw,CopyFromParent,
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Om knappen har ram skall den ocksï¿½ kunna clickas i och
***highligtas.
***Utan ram ï¿½r den bara en "label".
*/
    if ( bw > 0 ) XSelectInput(xdisp,xwin_id,ButtonPressMask | ButtonReleaseMask |
                                             EnterWindowMask | LeaveWindowMask);
/*
***Skapa en WPBUTT.
*/
    if ( (butptr=(WPBUTT *)v3mall(sizeof(WPBUTT),"WPwcbu")) == NULL )
       return(erpush("WP1112",str1));

    butptr->id.w_id = (wpw_id)NULL;
    butptr->id.p_id = (wpw_id)NULL;
    butptr->id.x_id = xwin_id;

    butptr->geo.x =  x;
    butptr->geo.y =  y;
    butptr->geo.dx =  dx;
    butptr->geo.dy =  dy;
    butptr->geo.bw =  bw;

    butptr->color.bckgnd = cb;
    butptr->color.forgnd = cf;

    if ( strlen(str1) > 80 ) str1[80] = '\0';
    strcpy(butptr->stroff,str1);
    if ( strlen(str2) > 80 ) str2[80] = '\0';
    strcpy(butptr->stron,str2);

    butptr->status = FALSE;
    butptr->hlight = FALSE;

    if ( fstr[0] == '\0' ) butptr->font = 0;
    else if ( (butptr->font=WPgfnr(fstr)) < 0 )
                         return(erpush("WP1102",fstr));
/*
***Init tooltip text.
*/
    butptr->tt_str[0] = '\0';
/*
***Return ptr to button.
*/
   *outptr = butptr;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool    WPxpbu(
        WPBUTT *butptr)

/*      Expose handler for WPBUTT.
 *
 *      In: buttptr = C ptr to WPBUTT.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1996-12-12 Vï¿½nsterjust. lablar, J.Kjellander
 *      1998-03-27 OpenGL för AIX, J.Kjellander
 *
 ******************************************************!*/

  {
    int          x,y;
    char         text[81];
    GC           but_gc;
    WPRWIN      *rwinpt;
    XFontStruct *xfs;

/*
***Vilken text skall fï¿½nstret innehï¿½lla ?
*/
    if ( butptr->status ) strcpy(text,butptr->stron);
    else                  strcpy(text,butptr->stroff);
/*
***Om det ï¿½r en knapp med ram, berï¿½kna textens lï¿½ge sï¿½
***att den hamnar mitt i fï¿½nstret.
*/
    x = (butptr->geo.dx - WPstrl(text))/2;
    y =  WPftpy(butptr->geo.dy);
/*
***Vilket GC skall anvï¿½ndas ? Om knappen sitter i
***ett WPRWIN mï¿½ste vi anvï¿½nda  dess GC eftersom
***knappen isï¿½fall delar dess visual. Knappar i
***ï¿½vriga typer av fï¿½nster kan anvï¿½nda xgc.
*/
    switch ( wpwtab[butptr->id.p_id].typ )
      {
      case TYP_RWIN:
      rwinpt = (WPRWIN *)wpwtab[butptr->id.p_id].ptr;
      but_gc = rwinpt->win_gc;
      xfs = WPgfnt(0);       /*****ny****/
      XSetFont(xdisp,but_gc,xfs->fid);    /*******ny*******/
      break;

      default:
      but_gc = xgc;
      WPsfnt(butptr->font);       /*****ny****/
      break;
      }
/*
***Set the button backgrund color and the background for writing.
*/
    XSetWindowBackground(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,butptr->color.bckgnd));
    XSetBackground(xdisp,but_gc,WPgcbu(butptr->id.p_id,butptr->color.bckgnd));
/*
***Set the forground color for writing and write.
*/
    XSetForeground(xdisp,but_gc,WPgcbu(butptr->id.p_id,butptr->color.forgnd));
    XDrawImageString(xdisp,butptr->id.x_id,but_gc,x,y,text,strlen(text));
/*
***Tills vidare ï¿½terstï¿½ller vi aktiv font och
***fï¿½rger till default igen.
*/
    WPsfnt(0);

    if ( butptr->color.bckgnd != WP_BGND2 )
      XSetBackground(xdisp,but_gc,WPgcbu(butptr->id.p_id,WP_BGND2));
    if ( butptr->color.forgnd != WP_FGND )
      XSetForeground(xdisp,but_gc,WPgcbu(butptr->id.p_id,WP_FGND));
/*
***Test av 3D-ram.
*/
    if ( butptr->geo.bw > 0 )
      WPd3db((char *)butptr,TYP_BUTTON);
/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        void    WPscbu(
        WPBUTT *butptr,
        int     color)

/*      Sets the background color of a button.
 *
 *      In: buttptr = C-ptr to WPBUTT.
 *          color   = Color number.
 *
 *      (C)2007-03-24 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Set the button backgrund color and force an expose
***to make the new color visible.
*/
   butptr->color.bckgnd = color;

   XUnmapWindow(xdisp,butptr->id.x_id);

   XSetWindowBackground(xdisp,butptr->id.x_id,
                        WPgcbu(butptr->id.p_id,butptr->color.bckgnd));

   XMapWindow(xdisp,butptr->id.x_id);
  }

/********************************************************/
/*!******************************************************/

        bool WPbtbu(
        WPBUTT *butptr)

/*      Button handler for WPBUTT.
 *
 *      In: buttptr = C ptr to WPBUTT.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Toggle the button status.
*/
    if ( butptr->status == FALSE )
      {
      butptr->status = TRUE;
/*      WPscbu(butptr,WP_BGND3); */
      }
    else
      {
      butptr->status = FALSE;
/*      WPscbu(butptr,WP_BGND2); */
      }
/*
***Erase window and expose again.
*/
    XClearWindow(xdisp,butptr->id.x_id);
    WPxpbu(butptr);
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool    WPcrbu(
        WPBUTT *butptr,
        bool    enter)

/*      Crossing handler for WPBUTT.
 *
 *      In: butptr = C ptr to WPBUTT.
 *          enter  = TRUE  => Enter.
 *                   FALSE => Leave.
 *
 *      Return: Always = TRUE.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2007-03-08 Tooltips, J.Kjellander
 *
 ******************************************************!*/

 {
   int x,y;

/*
***Enter => Change color of window border to WP_NOTI.
*/
   if ( enter == TRUE )
     {
     XSetWindowBorder(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,WP_NOTI));
     butptr->hlight = TRUE;
/*
***Order a tooltip in a few seconds if there is one to display.
*/
     if ( butptr->tt_str[0] != '\0' )
       {
       WPgtmp(&x,&y);
       WPorder_tooltip(x+5,y+10,butptr->tt_str);
       }
     }
/*
***Leave => Reset window border color to WP_BGND.
*/
   else                            
     {
     XSetWindowBorder(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,WP_BGND1));
     butptr->hlight = FALSE;
/*
***Remove ordered or active tooltip.
*/
     WPclear_tooltip();
     }

   return(TRUE);
 }

/********************************************************/
/*!******************************************************/

        short WPgtbu(
        DBint  iwin_id,
        DBint  butt_id,
        DBint *status)

/*      Get-rutin fï¿½r WPBUTT.
 *
 *      In: iwin_id = Huvudfï¿½nstrets id.
 *          butt_id = Button-fï¿½nstrets id.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1122 = Fï¿½rï¿½ldern %s finns ej.
 *              WP1132 = Fï¿½rï¿½ldern %s ej WPIWIN.
 *              WP1142 = Knappen %s finns ej.
 *              WP1152 = %s ï¿½r ej en knapp.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char    errbuf[80];
    WPWIN  *winptr;
    WPIWIN *iwinptr;
    WPBUTT *buttptr;

/*
***Fixa C-pekare till fï¿½rï¿½lderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1122",errbuf));
      }
/*
***Kolla att det ï¿½r ett WPIWIN.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1132",errbuf));
      }
/*
***Fixa en C-pekare till WPIWIN.
*/
    iwinptr = (WPIWIN *)winptr->ptr;
/*
***Kolla om subfï¿½nstret med angivet id finns och ï¿½r
***av rï¿½tt typ.
*/
    if ( iwinptr->wintab[(wpw_id)butt_id].ptr == NULL )
      {
      sprintf(errbuf,"%d",(int)butt_id);
      return(erpush("WP1142",errbuf));
      }

    if ( iwinptr->wintab[(wpw_id)butt_id].typ != TYP_BUTTON )
      {
      sprintf(errbuf,"%d",(int)butt_id);
      return(erpush("WP1152",errbuf));
      }
/*
***Fixa en C-pekare till WPBUTT.
*/
    buttptr = (WPBUTT *)iwinptr->wintab[(wpw_id)butt_id].ptr;
/*
***Returnera status.
*/
    *status = buttptr->status;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPdlbu(
        WPBUTT *butptr)

/*      Dï¿½dar en WPBUTT.
 *
 *      In: buttptr = C-pekare till WPBUTT.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Check for active tooltip and delete if nessecary.
*/
   WPclear_tooltip();
/*
***Release allocated C memory.
*/
   v3free((char *)butptr,"WPdlbu");
   return(0);
  }

/********************************************************/
/*!******************************************************/

 unsigned long WPgcbu(
        wpw_id p_id,
        int    colnum)

/*      Returnerar fï¿½rg fï¿½r WPBUTT. Om fï¿½rï¿½ldern ï¿½r
 *      en WPRWIN gï¿½rs sï¿½rskild hantering.
 *
 *      In: p_id   = ID fï¿½r fï¿½rï¿½lder.
 *          colnum = VARKON fï¿½rgnummer.
 *
 *      FV: Pixelvï¿½rde.
 *
 *      (C)microform ab 1998-03-27 J. Kjellander
 *
 *      2007-04-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   WPRWIN *rwinpt;

/*
***Om knappen sitter i ett WPRWIN mï¿½ste vi returnera
***ett pixelvï¿½rde som ï¿½r kompatibelt med det fï¿½nstrets
***visual. ï¿½vriga fï¿½nster anvï¿½nder default visual.
*/
   switch ( wpwtab[p_id].typ )
     {
     case TYP_RWIN:
     rwinpt   = (WPRWIN *)wpwtab[p_id].ptr;
     switch ( colnum )
       {
       case WP_BGND1: return(rwinpt->bgnd1);
       case WP_BGND2: return(rwinpt->bgnd2);
       case WP_BGND3: return(rwinpt->bgnd3);
       case WP_FGND:  return(rwinpt->fgnd);
       case WP_TOPS:  return(rwinpt->tops);
       case WP_BOTS:  return(rwinpt->bots);
       case WP_NOTI:  return(rwinpt->noti);
       default:       return(0);
       }
     break;

     default:
     return(WPgcol(colnum));
     }
  }

/********************************************************/
