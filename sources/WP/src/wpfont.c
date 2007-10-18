/**********************************************************************
*
*    wpfont.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPfini();   Init font handling
*    WPgfnr();   Map font name -> number
*    WPgfnt();   Returns XFont pointer
*    WPsfnt();   Sets active font
*
*    WPstrl();   Length of string with active font
*    WPgtsl();   Length of string with specified font
*    WPstrh();   Height of active font
*    WPgtsh();   Height of any font
*    WPftpy();   Center text vertically
*
*    WPwstr();   Display text in window
*    WPdivs();   Split long line of text into two lines
*
*    WPd3db();   Draw window 3D border
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

#define FNTTBSIZ 20

typedef struct
{
char        namn[V3STRLEN+1];
XFontStruct *xfs;
}WPFONT;

static short  actfnt;
static WPFONT fnttab[FNTTBSIZ];

/* fnttab ï¿½r en tabell med information om laddade fonter.
   Alla namn initieras till "" och alla XFS:s till NULL.
   actfnt hï¿½ller reda pï¿½ vilken font i fnttab som ï¿½r aktiv
   dvs. ingï¿½r i aktiv GC. Font nummer 0 laddas som default
   font av WPfini() vid uppstart. ï¿½vriga fonter laddas dï¿½
   olika typer av fï¿½nster, knappar, ikoner etc. skapas av V3
   sjï¿½lvt eller via MBS.
*/

XFontStruct *xfs;             /* Tills vidare bara !!!! */

/*!******************************************************/

        short WPfini()

/*      Init-rutin fï¿½r font-hanteringen. Anropas av
 *      WPinit() vid uppstart. Laddar default font
 *      och gï¿½r den till font nr: 0.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: WP1002 = Kan ej ladda fonten %s.
 *
 *      (C)microform ab 13/12/93 J. Kjellander
 *    
 *      1/11-94  VAX-defaltfont, J. Kjellander
 *
 ******************************************************!*/

  {
    short    i;
    char     fntnam[V3STRLEN+1],*type[20];
    XrmValue value;

/*
***Nollställ fnttab.
*/
    for ( i=0; i<FNTTBSIZ; ++i )
      {
      fnttab[i].namn[0] = '\0';
      fnttab[i].xfs     = NULL;
      }
/*
***Hï¿½mta fontnamn frï¿½n resursdatabasen.
*/
    if ( XrmGetResource(xresDB,"varkon.font","Varkon.Font",
                        type,&value) ) strcpy(fntnam,value.addr);
#ifdef UNIX
    else strcpy(fntnam,"fixed");
#endif
/*
***Prova att ladda.
*/
    if ( WPgfnr(fntnam) < 0 ) return(erpush("WP1002",fntnam));
/*
***Allt gick bra, dï¿½ gï¿½r vi den aktiv ocksï¿½. actfnt = -1
***tvingar WPsfnt() att aktivera.
*/
    actfnt = -1;
    WPsfnt((short)0);
/*
***Ladda fonten "cursor" som font 1.
*/
    strcpy(fntnam,"cursor");
    if ( WPgfnr(fntnam) < 0 ) return(erpush("WP1002",fntnam));
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPgfnr(
        char *fntnam)

/*      Laddar fonten fntnam om den inte redan ï¿½r
 *      laddad och returnerar en siffra som anger
 *      dess plats i fnttab.
 *
 *      In: fntnam = Fontnamn.
 *
 *      Ut: Inget.
 *
 *      FV: Giltigt entry i fnttab eller -1.
 *
 *      Felkoder : WP1012 = fnttab full, font = %s.
 *                 WP1022 = Fonten %s finns ej.
 *
 *      (C)microform ab 12/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short        i;
    XFontStruct *f;

/*
***Bï¿½rja med att kolla om fonten redan ï¿½r laddad.
*/
    for ( i=0; i<FNTTBSIZ; ++i )
      if ( strcmp(fnttab[i].namn,fntnam) == 0  ) return(i);
/*
***Ej laddad, leta upp ledig plats i fnttab.
*/
    for ( i=0; i<FNTTBSIZ; ++i ) if ( fnttab[i].xfs == NULL ) break;
    if ( i == FNTTBSIZ ) return(erpush("WP1012",fntnam));
/*
***Prova att ladda fonten.
*/
    if ( (f=XLoadQueryFont(xdisp,fntnam)) == NULL )
      return(erpush("WP1022",fntnam));
/*
***Allt vï¿½l, lagra fontnam och pekare i tabellen.
*/
    strcpy(fnttab[i].namn,fntnam);
    fnttab[i].xfs = f;
/*
***Slut, returnera nytt fontnummer.
*/
    return(i);
  }

/********************************************************/
/*!******************************************************/

        XFontStruct *WPgfnt(
        short fntnum)

/*      Returnerar X-fontpekare till font med visst
 *      nummer.
 *
 *      In: fntnum = Fontnummer.
 *
 *      Ut: Inget.
 *
 *      FV: X-Fontpekare.
 *
 *      (C)microform ab 12/12/93 J. Kjellander
 *
 *
 ******************************************************!*/

  {
    return(fnttab[fntnum].xfs);
  }

/********************************************************/
/*!******************************************************/

        short WPsfnt(
        short fntnum)

/*      Sï¿½tter aktiv font.
 *
 *      In: fntnum = Fontnummer.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 12/12/93 J. Kjellander
 *
 *
 ******************************************************!*/

  {
    if ( fntnum != actfnt )
      {
      actfnt = fntnum;
      XSetFont(xdisp,xgc,fnttab[fntnum].xfs->fid);
      xfs = fnttab[fntnum].xfs;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        int WPstrl(
        char *fstring)

/*      Returnerar lï¿½ngden pï¿½ en strï¿½ng i pixels med aktiv font.
 *
 *      In: fontstring.
 *
 *      Ut: Inget.
 *
 *      FV: Lï¿½ngden pï¿½ strï¿½ngen i pixels.
 *
 *      (C)microform ab 23/6/92 U.Andersson
 *
 *
 ******************************************************!*/

  {
    return(XTextWidth(xfs,fstring,strlen(fstring)));
  }

/********************************************************/
/*!******************************************************/

        short WPgtsl(
        char *str,
        char *font,
        int  *plen)

/*      Rï¿½knar ut lï¿½ngden pï¿½ en strï¿½ng i pixels om den
 *      skulle skrivas ut med viss font.
 *
 *      In: str  = Text att skriva ut.
 *          font = Fontens namn.
 *
 *      Ut: *plen = Lï¿½ngd i pixels.
 *
 *      (C)microform ab 1996-05-21 J.Kjellander
 *
 ******************************************************!*/

  {
   short        fntnum;
   XFontStruct *fs;
/*
***Om det gï¿½ller aktiv font tar vi den globala variablen xfs.
*/
   if ( *font == '\0' ) fs = xfs;
/*
***Annars tar vi den frï¿½n fonttabellen. Finns den inte dï¿½r
***laddas den av WPgfnr() automatisk. Om fonten inte finns
***ï¿½verhuvudtaget eller om fonttabellen ï¿½r full returneras
***negativt status.
*/
   else
     {
     if ( (fntnum=WPgfnr(font)) < 0 ) return(fntnum);
     else                             fs = WPgfnt(fntnum);
     }
/*
***Nu har vi en fontpekare och kan berï¿½kna textlï¿½ngden.
*/
   *plen = XTextWidth(fs,str,strlen(str));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        int WPstrh()

/*      Rï¿½knar ut hï¿½jden pï¿½ en strï¿½ng i pixels.
 *
 *      In: fontstring.
 *
 *      Ut: Inget.
 *
 *      FV: Hï¿½jden pï¿½ strï¿½ngen i pixels.
 *
 *      (C)microform ab 23/6/92 U.Andersson
 *
 *
 ******************************************************!*/

  {
    return(xfs->ascent + xfs->descent);
  }

/********************************************************/
/*!******************************************************/

        short WPgtsh(
        char *font,
        int  *phgt)

/*      Berï¿½knar hï¿½jden pï¿½ en viss font.
 *
 *      In: font = Fontens namn.
 *
 *      Ut: *phgt = Hï¿½jd i pixels.
 *
 *      (C)microform ab 1996-05-21 J.Kjellander
 *
 ******************************************************!*/

  {
   short        fntnum;
   XFontStruct *fs;
/*
***Om det gï¿½ller aktiv font tar vi den globala variablen xfs.
*/
   if ( *font == '\0' ) fs = xfs;
/*
***Annars tar vi den frï¿½n fonttabellen. Finns den inte dï¿½r
***laddas den av WPgfnr() automatisk. Om fonten inte finns
***ï¿½verhuvudtaget eller om fonttabellen ï¿½r full returneras
***negativt status.
*/
   else
     {
     if ( (fntnum=WPgfnr(font)) < 0 ) return(fntnum);
     else                             fs = WPgfnt(fntnum);
     }
/*
***Nu har vi en fontpekare och kan berï¿½kna hï¿½jden.
*/
   *phgt = fs->ascent + fs->descent;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        int WPftpy(
        int dy)

/*      Rï¿½knar ut y-koordinaten fï¿½r en text i pixels.
 *
 *      In: dy = hï¿½jden fï¿½r textfï¿½nstret.
 *
 *      Ut: Inget.
 *
 *      FV: Y-koordinaten i pixels.
 *
 *      (C)microform ab 23/6/92 U.Andersson
 *
 *
 ******************************************************!*/

  {
    return((dy - xfs->descent + xfs->ascent)/2);
  }

/********************************************************/
/*!******************************************************/

        short WPwstr(
        Window wid,
        int    x,
        int    y,
        char  *s)

/*      Lï¿½gnivï¿½-rutin fï¿½r utskrift av text i fï¿½nster.
 *      Textfont, fï¿½rg mm. stï¿½lls in innan denna rutin
 *      anropas.
 *
 *      In: wid = Fï¿½nster att skriva texten i.
 *          x,y = Position relativt fï¿½nstret.
 *          s   = Pekare till NULL-terminerad strï¿½ng.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 17/8/92 J. Kjellander
 *
 *
 ******************************************************!*/

  {

/*
***Skriv ut.
*/
    XDrawImageString(xdisp,wid,xgc,x,y,s,strlen(s));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPdivs(
        char text[],
        int  maxpix,
        int  *tdx,
        int  *tdy,
        char str1[],
        char str2[])

/*      Delar en textstrï¿½ng i tvï¿½ delar.
 *      Berï¿½knar platsbehov i pixels (x-och y-led)
 *
 *      In: text   = textstrï¿½ng som ska anvï¿½ndas.
 *          maxpix = max antal pixels som det aktiva fï¿½nstret innehï¿½ller.
 *
 *      Ut: tdx  = textbredd i pixels.
 *          tdy  = texthï¿½jd i pixels.
 *          str1 = textstrï¿½ng1.
 *          str2 = textstrï¿½ng2. 
 *
 *      FV: 0.
 *
 *      (C)microform ab 23/6/92 U.Andersson
 *
 *      15/2/95 Bug, J. Kjellander
 *
 ******************************************************!*/

  {
    int sl,slm;

/*
***Berï¿½kna antalet tecken som texten innehï¿½ller 
***och hur mï¿½nga pixels detta ï¿½r.
*/
    sl = strlen(text);   
   *tdx = WPstrl(text); 
/*
***textstrï¿½ng <= max antal pixels som texten 
***fï¿½r ta med hï¿½nsyn till aktuell skï¿½rmbredd.
*/
    if ( *tdx <= maxpix )    
      {
      strcpy(str1,text);
      strcpy(str2,"");
      *tdy = WPstrh();
      }
/*
***textstrï¿½ng > max antal pixels som texten 
***fï¿½r ta med hï¿½nsyn till aktuell skï¿½rmbredd.
*/
    else                  
      {
/*
***Bï¿½rja mitt i strï¿½ngen, gï¿½ ï¿½t hï¿½ger och leta upp fï¿½rsta mellanslag.
*/
      slm = sl/2;

      while( slm < sl  &&  text[slm] != ' ') ++slm; 
/*
***Om det fanns ett delar vi strï¿½ngen dï¿½r ?
*/ 
      if ( slm < sl )
        {
        text[slm] = '\0';
        strcpy(str1,text);
        strcpy(str2,&text[slm+1]);
       *tdx = WPstrl(str1);
       *tdy = 2*WPstrh();
        }
/*
***Om inte bï¿½rjar vi om i mitten och letar ï¿½t vï¿½nster istï¿½llet.
*/
      else
        {
        slm = sl/2;
        while( slm > 0  &&  text[slm] != ' ') --slm; 

        if ( slm > 0 )
          {
          text[slm] = '\0';
          strcpy(str1,text);
          strcpy(str2,&text[slm+1]);
         *tdx = WPstrl(str2);
         *tdy = 2*WPstrh();
          }
/*
***Om inget mellanslag hittas dï¿½r heller delar vi strï¿½ngen mitt av.
*/
        else
          {
          slm = sl/2;
          text[slm] = '\0';
          strcpy(str1,text);
          strcpy(str2,&text[slm+1]);
         *tdx = WPstrl(str1);
         *tdy = 2*WPstrh();
          }
        }
      }

    return(0); 
  }

/********************************************************/
/*!******************************************************/

        short WPd3db(
        char  *winptr,
        int    wintyp)

/*      Lï¿½gnivï¿½-rutin fï¿½r att fï¿½rse ett fï¿½nster med 
 *      en 3D-ram.
 *
 *      In: winptr = Pekare till WPBUTT, WPEDIT eller WPICON.
 *          wintyp = Typ av fï¿½nster, TYP_EDIT, TYP_BUTTON
 *                                   TYP_ICON.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 19/1/94 J. Kjellander
 *
 *      1998-03-29 OpenGL fï¿½r AIX, J.Kjellander
 *
 ******************************************************!*/

  {
    int     dx,dy,bw,i;
    short   bc,upleft,dnrgth;
    Window  xid;
    GC      act_gc;
    WPBUTT *butptr;
    WPEDIT *edtptr;
    WPICON *icoptr;
    WPRWIN *rwinpt;
    unsigned long bgpix,fgpix,ulpix,drpix;

/*
***Vilken sorts fï¿½nster ï¿½r det ? Ta reda pï¿½ storlek etc.
*/
    switch ( wintyp )
      {
      case TYP_BUTTON:
      butptr = (WPBUTT *)winptr;
      if ( wpwtab[butptr->id.p_id].typ  ==  TYP_RWIN )
        {
        rwinpt = (WPRWIN *)wpwtab[butptr->id.p_id].ptr;
        act_gc = rwinpt->win_gc;
        }
      else act_gc = xgc;

      xid    = butptr->id.x_id;
      dx     = butptr->geo.dx;
      dy     = butptr->geo.dy;
      bw     = butptr->geo.bw;
      bc     = butptr->color.bckgnd;
      upleft = WP_TOPS;
      dnrgth = WP_BOTS;
      ulpix  = WPgcbu(butptr->id.p_id,WP_TOPS);
      drpix  = WPgcbu(butptr->id.p_id,WP_BOTS);
      bgpix  = WPgcbu(butptr->id.p_id,WP_BGND1);
      fgpix  = WPgcbu(butptr->id.p_id,WP_FGND);
      break;

      case TYP_EDIT:
      edtptr = (WPEDIT *)winptr;
      act_gc = xgc;
      xid    = edtptr->id.x_id;
      dx     = edtptr->geo.dx;
      dy     = edtptr->geo.dy;
      bw     = edtptr->geo.bw;
      bc     = WP_BGND2;
      upleft = WP_BOTS;
      dnrgth = WP_TOPS;
      ulpix  = WPgcol(WP_BOTS);
      drpix  = WPgcol(WP_TOPS);
      bgpix  = WPgcol(WP_BGND2);
      fgpix  = WPgcol(WP_FGND);
      break;

      case TYP_ICON:
      icoptr = (WPICON *)winptr;
      act_gc = xgc;
      xid    = icoptr->id.x_id;
      dx     = icoptr->geo.dx;
      dy     = icoptr->geo.dy;
      bw     = icoptr->geo.bw;
      bc     = WP_BGND2;
      upleft = WP_TOPS;
      dnrgth = WP_BOTS;
      ulpix  = WPgcol(WP_TOPS);
      drpix  = WPgcol(WP_BOTS);
      bgpix  = WPgcol(WP_BGND2);
      fgpix  = WPgcol(WP_FGND);
      break;

      default:
      return(0);
      }
/*
***Rita. Antal pixels = dx,dy men adresserna = 0 till dx-1.
*/
    dx -= 1;
    dy -= 1;
/*
***Skugga ï¿½ver och till vï¿½nster.
*/
    if ( upleft != WP_FGND )
      XSetForeground(xdisp,act_gc,ulpix);

    for ( i=1; i<bw+1; ++i )
      {
      XDrawLine(xdisp,xid,act_gc,i,i,dx-i,i);
      XDrawLine(xdisp,xid,act_gc,i,i,i,dy-i);
      }
/*
***Om knappen har annorlunda bakgrundsfï¿½rg ï¿½n huvud-
***fï¿½nstret mï¿½ste en ram pï¿½ 1 pixel med bakgrundsfï¿½rg
***skapas fï¿½rst. ( Luften mellan 3D-ramen och Notify-ramen ).
***Detta gï¿½rs hï¿½r (mellan upleft och dnrgth) fï¿½r att minimera
***antalet anrop till XSetForeground().
*/
    if ( bc != WP_BGND1 )
      {
      XSetForeground(xdisp,act_gc,bgpix);
      XDrawLine(xdisp,xid,act_gc,0,0,dx,0);
      XDrawLine(xdisp,xid,act_gc,dx,0,dx,dy);
      XDrawLine(xdisp,xid,act_gc,dx,dy,0,dy);
      XDrawLine(xdisp,xid,act_gc,0,dy,0,0);
      }
/*
***Skugga under och till hï¿½ger.
*/
    XSetForeground(xdisp,act_gc,drpix);

    for ( i=1; i<bw+1; ++i )
      {
      XDrawLine(xdisp,xid,act_gc,dx-i,i,dx-i,dy-i);
      XDrawLine(xdisp,xid,act_gc,i,dy-i,dx-i,dy-i);
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
