/**********************************************************************
*
*    wptext.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPdrtx();    Draw text
*    WPdltx();    Delete text
*    WPpltx();    Create 3D polyline
*    WPprtx();    Project text polyline
*
*    WPinfn();    Init fonts
*    WPldfn();    Load font file
*    WPexfn();    Exit fonts
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
#include "../../GE/include/GE.h"
#include "../include/WP.h"
#include "../include/font0.h"
#include <math.h>

extern int actpen;

static short drawtx(WPGWIN *gwinpt, DBText *txtpek, char *strpek,
                    DBptr la, bool draw);
static short make_npoly(unsigned char *strpek, int font, double h,
                        double b, double slant, int *npts,
                        double x[], double y[], char a[]);
/*
***The font table.
*/
struct font
{
short          *antpek;   /* Ptr to font anttab */
short          *pekpek;   /* Ptr to font pektab */
unsigned short *vekpek;   /* Ptr to font vektab */
bool            loaded;   /* Loaded/Not loaded */
bool            malloc;   /* Memory allocated/Not allocated */
};

#define FNTMAX 20

static struct font fnttab[FNTMAX];    /* All loaded fonts */
static int    actfnt;                 /* Currently active font */

/* Needed for polyline creation */

static short *ppektb,*panttb;
static unsigned short *pvektb;

/*!******************************************************/

        short   WPdrtx(
        DBText *txtpek,
        char   *strpek,
        DBptr   la,
        DBint   win_id)

/*      Display a text.
 *
 *      In: txtpek => Pekare till text-post.
 *          strpek => Pekare till str�ng.
 *          la     => GM-adress.
 *          win_id => F�nster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 22/1/95 J. Kjellander
 *
 *      1997-12-25 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om texten �r blankad �r det enkelt.
*/
   if ( txtpek->hed_tx.blank) return(0);
/*
***Loopa igenom alla WPGWIN-f�nster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi rita i detta f�nster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja, ligger texten p� en niv� som �r t�nd i detta f�nster ?
*/
         if ( WPnivt(gwinpt->nivtab,txtpek->hed_tx.level) )
           {
/*
***Ja. Kolla att r�tt f�rg �r inst�lld.
*/
           if ( txtpek->hed_tx.pen != actpen ) WPspen(txtpek->hed_tx.pen);
           if ( txtpek->wdt_tx != 0.0 ) WPswdt(gwinpt->id.w_id,txtpek->wdt_tx);
/*
***Sen �r det bara att rita.
*/
           drawtx(gwinpt,txtpek,strpek,la,TRUE);
           if ( txtpek->wdt_tx != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short   WPdltx(
        DBText *txtpek,
        char   *strpek,
        DBptr   la,
        DBint   win_id)

/*      Delete a text.
 *
 *      In: txtpek => Pekare till text-post.
 *          strpek => Pekare till str�ng.
 *          la     => GM-adress.
 *          win_id => F�nster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 21/1/95 J. Kjellander
 *
 *      1997-12-25 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-f�nst.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi sudda i detta f�nster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja. Om den finns i DF kan vi sudda snabbt.
*/
         if ( WPfobj(gwinpt,la,TXTTYP,&typ) )
           {
           if ( txtpek->wdt_tx != 0.0 ) WPswdt(gwinpt->id.w_id,txtpek->wdt_tx);
           WPdobj(gwinpt,FALSE);
           WProbj(gwinpt);
           if ( txtpek->wdt_tx != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
/*
***Om den nu ligger p� en sl�ckt niv� eller �r blankad g�r vi
***inget mer. Annars f�r vi �terskapa polylinjen och sudda l�ngsamt.
*/
         else
           {
           if ( !WPnivt(gwinpt->nivtab,txtpek->hed_tx.level)  ||
                               txtpek->hed_tx.blank) return(0);
           else
             {
             if ( txtpek->wdt_tx != 0.0 ) WPswdt(gwinpt->id.w_id,txtpek->wdt_tx);
             drawtx(gwinpt,txtpek,strpek,la,FALSE);
             if ( txtpek->wdt_tx != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
             }
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short   drawtx(
        WPGWIN *gwinpt,
        DBText *txtpek,
        char   *strpek,
        DBptr   la,
        bool    draw)

/*      Draw/erase a text in a window.
 *      If draw, store in DF.
 *
 *      In: gwinpt => Pekare till f�nster.
 *          txtpek => Pekare till text-post.
 *          strpek => Pekare till str�ng.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/1/95 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    k;

/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;
   WPpltx(txtpek,(unsigned char *)strpek,&k,x,y,z,a);
/*
***Project to the view of the window. WPpply() can
***not be used since text has two projection modes (TPMODE).
*/
   WPprtx(gwinpt,txtpek,k+1,x,y,z);
/*
***Klipp polylinjen. Om den �r synlig (helt eller delvis ),
***rita den.
*/
   if ( WPcply(&gwinpt->vy.modwin,-1,&k,x,y,a) )
     {
     if ( draw  &&  txtpek->hed_tx.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,TXTTYP) ) WPdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

         short   WPpltx(
         DBText *txtpek,
unsigned char   *strpek,
         int    *k,
         double  x[],
         double  y[],
         double  z[],
         char    a[])

/*      Bygger upp en text i form av en 3D-polylinje.
 *      Vid anropet �r *k offset till sista ev. tidigare
 *      genererade vektor i xya. Vid avslut �r *k offset
 *      till sist genererade vektor va text.
 *
 *      In: txtpek:    Pekare till textstruktur
 *          strpek:    Pekare till textstr�ng
 *          k+1:       Offset till textstart
 * 
 *      Ut: k:         Offset till textslut
 *          x,y,z,a:   Koordinater och status
 *
 *      FV: 0
 *
 *      (C)microform ab 10/7-85 Ulf Johansson
 *
 *      28/3/89 Streck om liten, J. Kjellander
 *      24/9/92 Optimering, J. Kjellander
 *      2/11/92 Mer optimering, J. Kjellander
 *      2/1/93  Fonter, J. Kjellander
 *      14/2/95 VARKON_FNT, J. Kjellander
 *      20/4/95 Test av fontnummer, J. Kjellander
 *      1998-03-18 (char *)strpek f�r AIX, J.Kjellander
 *      1998-09-30 3D, J.Kjellander
 *      2006-12-17 Moved to WP, J.Kjellander
 *
 ******************************************************!*/

  {
   int      i,npts,n=0;
   double   xt,yt,cosvri,sinvri,tposx,tposy,tposz;
   double   xn[PLYMXV],yn[PLYMXV];
   DBCsys   csy;
   DBVector p;
   DBTmat   invmat;

/*
***Lite felkontroller.
*/
   if ( txtpek->pmod_tx < 0  ||  txtpek->pmod_tx > 1 )
     {
     erpush("GP2022","");
     txtpek->pmod_tx = 0;
     }
/*
***Generera normaliserad 2D polyline.
*/
   make_npoly(strpek,(int)txtpek->fnt_tx,txtpek->h_tx,txtpek->b_tx,
                     txtpek->l_tx,&npts,xn,yn,&a[*k+1]);
/*
***Om TPMODE = 0 skall den normaliserade polylinjen
***vridas och translateras i BASIC:s XY-plan, dvs. med den
***till basic transformerade positionen (crd_tx) och vinkeln
***(v_tx).
*/
   switch ( txtpek->pmod_tx )
     {
     case 0:
     if ( txtpek->v_tx != 0.0 )
       {
       cosvri = COS(txtpek->v_tx*DGTORD);
       sinvri = SIN(txtpek->v_tx*DGTORD);

       for ( i=0; i<npts; ++i )
         {
         xt = xn[i];
         yt = yn[i];
         xn[i] = xt*cosvri - yt*sinvri;
         yn[i] = yt*cosvri + xt*sinvri;
         }
       }

     n = *k;
     tposx = txtpek->crd_tx.x_gm;
     tposy = txtpek->crd_tx.y_gm;
     tposz = txtpek->crd_tx.z_gm;

     for ( i=0; i<npts; ++i )
       {
       x[++n] = xn[i] + tposx;
       y[  n] = yn[i] + tposy;
       z[  n] = tposz;
       }
     break;
/*
***Om TPMODE = 1 vrider och translaterar vi i
***det lokala systemet. Sen transformerar vi till
***BASIC.
*/
     case 1:
     if ( txtpek->lang_tx != 0.0 )
       {
       cosvri = COS(txtpek->lang_tx*DGTORD);
       sinvri = SIN(txtpek->lang_tx*DGTORD);

       for ( i=0; i<npts; ++i )
         {
         xt = xn[i];
         yt = yn[i];
         xn[i] = xt*cosvri - yt*sinvri;
         yn[i] = yt*cosvri + xt*sinvri;
         }
       }

     n = *k;
     tposx = txtpek->lpos_tx.x_gm;
     tposy = txtpek->lpos_tx.y_gm;

     for ( i=0; i<npts; ++i )
       {
       x[++n] = xn[i] + tposx;
       y[  n] = yn[i] + tposy;
       z[  n] = 0.0;
       }

     if ( txtpek->pcsy_tx )
       {
       n = *k;
       DBread_csys(&csy,NULL,txtpek->pcsy_tx);
       GEtform_inv(&csy.mat_pl,&invmat);

       for ( i=0; i<npts; ++i )
         {
         p.x_gm = x[++n];
         p.y_gm = y[  n];
         p.z_gm = z[  n];
         GEtfpos_to_local(&p,&invmat,&p);
         x[n] = p.x_gm;
         y[n] = p.y_gm;
         z[n] = p.z_gm;
         }
       }
     break;
     }
/*
***Slut.
*/
   *k = n;

   return(0); 
  }

/********************************************************/
/*!******************************************************/

        short   WPprtx(
        WPGWIN *gwinpt,
        DBText *txtpek,
        int     npts,
        double  x[],
        double  y[],
        double  z[])

/*      Projicerar en texts polylinje till aktivt vyplan.
 *
 *      In: txtpek = C-pekare till text.
 *          x,y,z  = Koordinater
 *          npts   = Antal positioner
 * 
 *      Ut: x,y,z = Nya koordinater
 *
 *      (C)microform ab 1998-10-01, J.Kjellander
 *
 *      1999-11-26 Z-Coordinates, J.Kjellander
 *      2006-12-17 Moved to WP, J.Kjellander
 *
 ******************************************************!*/

  {
   int      i;
   DBfloat  x0,y0,dx,dy;
   DBVector p;

/*
***Om TPMODE = 0 och det �r en 3D-vy skall bara
***startpositionen projiceras.
*/
    if ( txtpek->pmod_tx == 0 )
      { 
      p.x_gm = txtpek->crd_tx.x_gm;
      p.y_gm = txtpek->crd_tx.y_gm;
      p.z_gm = txtpek->crd_tx.z_gm;

      x0 = gwinpt->vy.matrix.k11 * p.x_gm +
           gwinpt->vy.matrix.k12 * p.y_gm +
           gwinpt->vy.matrix.k13 * p.z_gm;
      y0 = gwinpt->vy.matrix.k21 * p.x_gm +
           gwinpt->vy.matrix.k22 * p.y_gm +
           gwinpt->vy.matrix.k23 * p.z_gm;

      dx = x0 - p.x_gm;
      dy = y0 - p.y_gm;

      for ( i=0; i<npts; ++i )
        {
        x[i] += dx;
        y[i] += dy;
        }      
      }
/*
***Om TPMODE = 1 och det �r en 3D-vy projicerar vi
***hela polylinjen till vyplanet.
*/
    else
      {
      for ( i=0; i<npts; ++i )
        {
        p.x_gm = x[i];
        p.y_gm = y[i];
        p.z_gm = z[i];
        x[i] = gwinpt->vy.matrix.k11 * p.x_gm +
               gwinpt->vy.matrix.k12 * p.y_gm +
               gwinpt->vy.matrix.k13 * p.z_gm;
  
        y[i] = gwinpt->vy.matrix.k21 * p.x_gm +
               gwinpt->vy.matrix.k22 * p.y_gm +
               gwinpt->vy.matrix.k23 * p.z_gm;
            
        z[i] = gwinpt->vy.matrix.k31 * p.x_gm +
               gwinpt->vy.matrix.k32 * p.y_gm +
               gwinpt->vy.matrix.k33 * p.z_gm;
        }
      }

   return(0); 
  }

/********************************************************/
/*!******************************************************/

 static   short  make_npoly(
 unsigned char  *strpek,
          int    font,
          double h,
          double b,
          double slant,
          int   *npts,
          double x[],
          double y[],
          char   a[])

/*      Skapar en 2D polylinje av en str�ng. Vektorerna
 *      i polylinjen skalas till beg�rd storlek och
 *      skjuvas men inga andra transformationer g�rs. 
 *      Polylinjen b�rjar alltid i 0 och �r alltid
 *      horisontell med alla Z-koordinater implicit = 0.
 *
 *      In:
 *          strpek  = Pekare till textstr�ng.
 *          font    = �nskad textfont.
 *          h       = Texth�jd.
 *          b       = Textbredd.
 *          slant   = Skjuvning.
 *          minsize = Om mindre, g�r bara ett streck.
 * 
 *      Ut:
 *          *npts  = Antal punkter i polylinjen.
 *           x,y   = Koordinater
 *           a     = T�nd/sl�ck
 *
 *      FV: 0
 *
 *      (C)microform ab 1998-09-30, J.Kjellander
 *
 *      2006-12-17 Moved to WP, J.Kjellander
 *
 ******************************************************!*/

  {
   int     i,j,n,ntkn,veclim;
   DBint   ix,iy;
   DBfloat tposx,dtposx;
   DBfloat kh,kb,khl;
   char    path[V3PTHLEN+1];

/*
***Initiering.
*/
   tposx = 0.0;
/*
***Konstant f�r teckenpositionering i X-led.
*/
   dtposx = b*h*0.0166666667;
/*
***Antal tecken. (char *) �r f�r AIX.
*/
   ntkn = strlen((char *)strpek);
/*
***Om texten �r mindre �n minsize ritar vi den som
***ett streck.
*
   if ( ABS(h) < minsize )
     {
     a[0] = 0;
     x[0] = y[0] = 0.0;
     a[1] = VISIBLE;
     x[1] = ntkn*dtposx; y[1] = 0.0;
    *npts = 2;
     return(0); 
     }
*
***Texten �r s� stor att den syns.
***Konstanter f�r vektorgenerering.
***kh �r h�jdkonstanten, dvs. 1/10000.
***bk �r breddkonstanten, dvs. (b% * h)/6000.
***khl �r lutning.
*/
   kh  = h*0.0001;
   kb  = b*h*0.0000016666667; /* (b/100 * h)/6000 */
   khl = h*slant*0.000001;
/*
***Vilken font. Kolla att fontv�rdet �r rimligt. Mindre �n 0 eller
***st�rre �n 19 (FNTMAX=20) �r inte m�jligt h�r.
*/
   if ( font < 0  ||  font > FNTMAX-1 )  font = 0;

   if ( font != actfnt )
     {
     if ( fnttab[font].loaded != TRUE )
       {
       sprintf(path,"%s%d.FNT",IGgenv(VARKON_FNT),font);
       WPldfn(font,path);
       }
     actfnt = font;
     panttb = fnttab[font].antpek;
     ppektb = fnttab[font].pekpek;
     pvektb = fnttab[font].vekpek;
     }
/*
***Loop f�r alla tecken i texten.
*/
   n = 0;

   for ( i=0; i < ntkn; i++,n++ )
     {
     a[n] = 0; x[n] = tposx; y[n] = 0.0;
     j = ppektb[strpek[i]];
     veclim = j + 2*panttb[strpek[i]];
/*
***Loop f�r ett tecken.
***L�s ett koordinatpar ur vektab. Y flyttas till baslinjen.
*/
     while ( j < veclim )
       {
       ix = pvektb[j++];
       iy = pvektb[j++] - 5000;
/*
***Om X-koordinaten >= 32768 �r det en sl�ckt f�rflyttning
***till X - 32768, annars en t�nd f�rflyttning.
***F�r varje nytt tecken har det redan genererats en sl�ckt
***f�rflyttning till tecknets nollpunkt ovan. Om sj�lva
***tecknet dessutom b�rjar med en sl�ckt f�rflyttning kan
***vi sl� ihop dom till en enda sl�ckt f�rflyttning.
*/
       if ( ix > 32767 )
         {
         ix -= 32768;
         if ( a[n] > 0 ) a[++n] = 0; 
         }
       else
         {
         a[++n] = VISIBLE;
         }
/*
***Transformera till modell-koordinater.
*/
       x[n] = tposx + ix*kb + iy*khl;
       y[n] = iy*kh;

       if ( n >= (PLYMXV-2) ) break;          
       }           
/*
***Slut p� loopen f�r ett tecken.
***Inga tecken skall sluta med en sl�ckt f�rflyttning ! S� �r
***det tex. med mellanslag. 2/11/92 JK.
*/
     if ( a[n] == 0 ) --n;
/*
***Ber�kna n�sta teckens startposition.
*/
     tposx += dtposx;
/*
***Max antal vektorer f�r inte �verskridas.
*/
     if ( n >= (PLYMXV-3) ) break;          
     }
/*
***Slut.
*/
   *npts = n;

   return(0); 
  }

/********************************************************/
/*!******************************************************/

        short WPinfn()

/*      Nollst�ller fonttabellen. Denna rutin anropas
 *      av gpinit() innan n�gra fonter laddats. Minne f�r
 *      tidigare laddade fonter �terl�mnas ej, se gpexfn().
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 26/12/92 J. Kjellander
 *
 *      2006-12-17 Moved to WP, J.Kjellander
 *
 ******************************************************!*/

 {
    int i;

/*
***S�tt actfnt till n�got otill�tet s� att 1:a anropet
***till gppltx() s�ker byter till en vettig och d�rmed
***laddar font och s�tter upp vettiga font-pekare.
*/
    actfnt = -1;
/*
***Nollst�ll fonttabellen.
*/
    for ( i=0; i<FNTMAX; ++i )
      {
      fnttab[i].loaded = FALSE;
      fnttab[i].malloc = FALSE;
      }
/*
***Slut.
*/
    return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPldfn(
        int    font,
        char  *filnam)

/*      Laddar font fr�n fil. Om filen inte finns eller
 *      minne ej kan allokeras laddas V3:s default font
 *      som font nr: font.
 *
 *
 *      In:  font   = Fontnummer.
 *           filnam = Fontfil.
 * 
 *      Ut:  Inget.
 *
 *      FV:  0 alltid.
 *
 *      Felkoder:       0 = Ok.
 *                 GP2012 = Kan ej allokera minne.
 *
 *      (C)microform ab 27/12/92 J. Kjellander
 *
 *      1999-03-18 Bugfix 255->256, J.Kjellander
 *      2006-12-17 Moved to WP, J.Kjellander
 *
 ******************************************************!*/

 {
    short          *ptr1,*ptr2;
    unsigned short *ptr3;
    short           i,offs;
    char            rad[81];
    int             ntkn,nvek,ix,iy;
    FILE           *f;

extern          short  anttab[],pektab[];
extern unsigned short vektab[];

/*
***�r font med detta nummer redan laddad ? Is�fall
***deallokerar vi det minne den upptog om s�nt har allokerats.
*/
    if ( fnttab[font].loaded == TRUE  &&  fnttab[font].malloc == TRUE )
      {
      v3free(fnttab[font].antpek,"WPldfn");
      v3free(fnttab[font].pekpek,"WPldfn");
      v3free(fnttab[font].vekpek,"WPldfn");
      }
/*
***�ppna filen.
*/
    if ( (f=fopen(filnam,"r")) == NULL ) goto error;
/*
***L�s antal tecken och vektorer.
*/
    fgets(rad,80,f);
    sscanf(rad,"%d",&ntkn);
    fgets(rad,80,f);
    sscanf(rad,"%d",&nvek);
/*
***Allokera minne.
*/
    if ( (ptr1=(short *)v3mall(256*sizeof(short),"WPldfn")) == NULL )
      {
      erpush("GP2012","");
      goto error;
      }
    if ( (ptr2=(short *)v3mall(256*sizeof(short),"WPldfn")) == NULL )
      {
      erpush("GP2012","");
      goto error;
      }
    if ( (ptr3=(unsigned short *)v3mall(2*(ntkn+nvek)*sizeof(short),
                                                     "gpldfn")) == NULL )
      {
      erpush("GP2012","");
      goto error;
      }
/*
***Allt gick bra, uppdatera fnttab.
*/
    fnttab[font].antpek = ptr1;
    fnttab[font].pekpek = ptr2;
    fnttab[font].vekpek = ptr3;
    fnttab[font].loaded = TRUE;
    fnttab[font].malloc = TRUE;
/*
***L�s tecken. 255->256 1999-03-18 JK.
*/
    offs = 0;

    for ( ntkn=0; ntkn<256; ++ntkn )
      {
      fgets(rad,80,f);
      sscanf(rad,"%d",&nvek);
      if ( nvek > 0 )
        {
        *ptr1 = (short)nvek + 1;
        *ptr2 = offs;
        for ( i=0; i<=nvek; ++i )
          {
          fgets(rad,80,f);
          sscanf(rad,"%d%d",&ix,&iy);
          ptr3[offs++] = (unsigned short)ix;
          ptr3[offs++] = (unsigned short)iy;
          }
        }
      else
        {
        *ptr1 = 0;
        *ptr2 = 0;
        }
      ++ptr1;
      ++ptr2;
      }
/*
***Slut.
*/
    fclose(f);
    return(0);
/*
***Felutg�ng. Ladda V3:s default-font ist�llet.
*/
error:
    fnttab[font].antpek = anttab;
    fnttab[font].pekpek = pektab;
    fnttab[font].vekpek = vektab;
    fnttab[font].loaded = TRUE;
    fnttab[font].malloc = FALSE;
    return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPexfn()

/*      Nollst�ller fonttabellen. L�mnar tillbaks minne
 *      f�r laddade fonter som har minne allokerat.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 26/12/92 J. Kjellander
 *
 *      2006-12-17 Moved to WP, J.Kjellander
 *
 ******************************************************!*/

 {
    int i;

/*
***Nollst�ll.
*/
    for ( i=0; i<FNTMAX; ++i )
      {
      if ( fnttab[i].loaded == TRUE  &&  fnttab[i].malloc == TRUE )
        {
        v3free(fnttab[i].antpek,"gpexfn");
        v3free(fnttab[i].pekpek,"gpexfn");
        v3free(fnttab[i].vekpek,"gpexfn");
        }
      else
        {
        fnttab[i].loaded = FALSE;
        fnttab[i].malloc = FALSE;
        }
      }
/*
***Nollst�ll actfnt.
*/
    actfnt = -1;

    return(0);
 }

/********************************************************/
