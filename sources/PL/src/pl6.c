/*!******************************************************************/
/*  File: pl6.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   plpoly();    Plott polyline                                    */
/*   plcwdt();    Selects new paint pattern                         */
/*   plcnpt();    Calculates number of paint strokes                */
/*   plclip();    Clip vector                                       */
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

extern double xmin,ymin,skala,vinkel,curnog,clipw[];
extern double ptabr[],ptabp[];
extern char pfrnam[];
extern bool clip;

#define VISIBLE  1           /* T�nd f�rflyttning */

/*
***Globala variabler f�r m�lning och klippning.
*/

int    npaint;
double actwdt,doffs,penwdt;
double xt[PLYMXV],yt[PLYMXV];
char   at[PLYMXV];

/*!******************************************************/

        short plpoly(
        short  n,
        double x[],
        double y[],
        char   a[])

/*      Transformerar och plottar en polyline.
 *
 *      IN:
 *         n:           Offset till sista vektorn i polylinjen.
 *         x,y,a:       x-,y-koordinater och status hos vektorerna 
 *                      i polylinjen
 *
 *      (C)microform ab 31/1/91 J. Kjellander
 *
 ******************************************************!*/

 {
   short  i;
   double cosv,sinv,xs,ys,vec[4];

/*
***Skapa transformerad polylinje xt,yt.
*/
   cosv = COS(vinkel*DGTORD);
   sinv = SIN(vinkel*DGTORD);

   for ( i=0; i<=n; ++i )
     {
     xs = skala * x[i];
     ys = skala * y[i];
     xt[i] = (xs*cosv - ys*sinv) - xmin;
     yt[i] = (xs*sinv + ys*cosv) - ymin;
     }
/*
***Plotta med klippning.
*/
     if ( clip ) 
       {
/*
***Klipp alla vektorer och plotta dom individuellt.
*/
       for ( i=0; i<n; ++i )
         { 
         vec[0] = xt[i]; vec[1] = yt[i];
         vec[2] = xt[i+1]; vec[3] = yt[i+1];
         if ( plclip(vec,clipw) >= 0  &&  (a[i+1]&VISIBLE ) == VISIBLE )
           {
           plmove(vec[0],vec[1]);
           pldraw(vec[2],vec[3]);
           }
         }
       }
/*
***Plotta utan klippning.
*/
     else
       {
       for ( i=0; i<=n; ++i )
         {
         if ( (a[i] & VISIBLE) == VISIBLE ) pldraw(xt[i],yt[i]);
         else                               plmove(xt[i],yt[i]);
         }
       }

   return(0);

 }

/******************************************************/
/*!******************************************************/

        short plcwdt(int pen)

/*      Huvudrutin f�r pennbyte. Om breda linjer �r aktivt,
 *      dvs. om pennbreddsfil angetts p� kommandoraden,
 *      ber�knas nytt m�lningsm�nster, annars anropas plchpn()
 *      f�r normalt pennbyte.
 *
 *      In: pen = Nytt pennummer, dvs linjebredd.
 *
 *      (C)microform ab 6/2/91 J. Kjellander
 *
 *      18/2/91 5% �verlapp, J. Kjellander
 *      1998-09-18 WIDTH, J.Kjellander
 *
 ******************************************************!*/

 {
   double width;

/*
***Om ingen pennbreddsfil getts byter vi bara penna
***som vanligt. Denna metod anv�nds d� man har en
***plotter med flera pennor och vill styra vilken
***penna som skall anv�ndas via VARKON:s PEN-attribut.
***M�lning kommer d� inte att ske.
*/
   if ( pfrnam[0] == '\0' ) plchpn(pen);
/*
***Breda linjer �r aktivt. Leta upp den beg�rda linjebredden
***i ritningens pennbreddstabell. Om pennummer > WPNMAX,
***prova att l�ta plottern ta den pennan. Is�fall sker
***heller ingen m�lning. Om pennummer < WPNMAX kan den
***ha ett vettigt entry i ritningens pennbreddsfil eller
***om inte s� �r motsvarande bredd i ptabr = 0.0 I vilket
***fall som helst s�tter vi actwdt = ptabr[pen].
*/
   else
     {
     if ( pen < WPNMAX ) width = ptabr[pen];
     else
       {
       plchpn(pen);
       return(0);
       }
/*
***Om actwdt nu �r st�rre �n noll skall nytt m�lnings-
***m�nster v�ljas, annars tar vi den beg�rda pennan.
*/
     if ( width == 0.0 )
       {
       plchpn(pen);
       npaint = 0;
       }
/*
***B�rja med att prova om det finns en penna som passar
***exakt till den beg�rda linjebredden.
*/
     else plcnpt(width);
     }

   return(0);
 }

/*!******************************************************/
/*!******************************************************/

        short plcnpt(double width)

/*      Ber�knar nytt m�lningsm�nster. S�tter actwdt,
 *      byter penna och s�tter npaint.
 *
 *      In: width = �nskad linjebredd.
 *
 *      (C)microform ab 1998-09-18 J. Kjellander
 *
 ******************************************************!*/

 {
   double minwdt;
   int    i,minpen=0;

/*
***S�tt actwdt.
*/
   actwdt = width;
/*
***Ber�kna npaint.
*/
   for ( i=0; i<WPNMAX; ++i )
     if ( ptabp[i] == actwdt )
       {
       plchpn(i);
       npaint = 0;
       return(0);
       }
/*
***S� var inte fallet. Leta ist�llet upp den smalaste pennan.
***Om denna �r f�r bred, ta den ialla fall.
*/
   minwdt = 1E10;
   for ( i=0; i<WPNMAX; ++i )
     if ( ptabp[i] > 0.0  &&  ptabp[i] < minwdt )
       {
       minwdt = ptabp[i];
       minpen = i;
       }
   if ( minwdt > actwdt )
     {
     plchpn(minpen);
     npaint = 0;
     return(0);
     }
/*
***Vi har nu konstaterat att minst en penna smalare �n actwdt
***finns att tillg�. B�rja med att prova om det finns n�gon
***penna som �r bred nog f�r bara 2 m�lningar. Om inte prova 
***med tre osv... Ett �verlapp p� 5% �r �nskv�rt f�r att s�ker-
***st�lla att sm� h�l inte uppst�r tex. i cirklar.
*/
   npaint = 2;
nploop:
   minwdt = actwdt;
   for ( i=0; i<WPNMAX; ++i )
     if ( ptabp[i] >= 1.05*actwdt/npaint  &&  ptabp[i] < minwdt ) 
       {
       minwdt = ptabp[i];
       minpen = i;
       }
   if ( minwdt < actwdt )
     {
     plchpn(minpen);
     penwdt = minwdt;
     doffs = (actwdt - penwdt)/(npaint-1);
     return(0);
     }
   else
     {
     ++npaint;
     goto nploop;
     }
/*
***Slut.
*/
   return(0);
 }

/******************************************************/
/*!******************************************************/

        short plclip(
        double *v,
        double *w)

/*      Klipper en vektor mot ett f�nster.
*
*       IN:
*            v: Vektor.
*            w: F�nster.
*
*       UT:
*            v: Vektor enl FV.
*         
*       FV:
*            -1: Vektorn �r oklippt och ligger utanf�r f�nstret
*             0: Vektorn �r oklippt och ligger innanf�r f�nstret.
*             1: Vektorn �r klippt i punkt 1 (start).
*             2: Vektorn �r klippt i punkt 2 (slut).
*             3: Vektorn �r klippt i b�de punkt 1 och punkt 2.
*
*
*       (c)microform ab  J. Kjellander efter grapac's klp().
*
********************************************************!*/

  {
    register double *p1,*p2,*win;
    short sts1,sts2;

        sts1 = sts2 = 0;
        p1 = v;
        p2 = p1 + 2;
        win = w;

        /* Om punkt 1 ligger utanf�r f�nstret, klipp till f�nsterkanten */

        if (*p1 < *win) {

            if (*p2 < *win)
                return(-1);          /* Hela vektorn v�nster om f�nstret */

            *(p1+1) += (*(p2+1) - *(p1+1))*(*win - *p1)/(*p2 - *p1);
            *p1 = *win;
            sts1 = 1;
        } else if (*p1 > *(win+2)) {

            if (*p2 > *(win+2))
                return(-1);               /* Hela vektorn h�ger om f�nstret */

            *(p1+1) += (*(p2+1) - *(p1+1))*(*(win+2) - *p1)/(*p2 - *p1);
            *p1 = *(win+2);
            sts1 = 1; 
        }

        if (*(p1+1) < *(win+1)) {

            if (*(p2+1) < *(win+1))
                return(-1);               /* Hela vektorn nedanf�r f�nstret */

            *p1 += (*p2 - *p1)*(*(win+1) - *(p1+1))/(*(p2+1) - *(p1+1));
            *(p1+1) = *(win+1);
            sts1 = 1;
        } else if (*(p1+1) > *(win+3)) {

            if (*(p2+1) > *(win+3))
                return(-1);               /* Hela vektor ovanf�r f�nstret */

            *p1 += (*p2 - *p1)*(*(win+3) - *(p1+1))/(*(p2+1) - *(p1+1));
            *(p1+1) = *(win+3);
            sts1 = 1;
        }

        if (sts1 == 1)                    /* Punkt 1 klippt */
            if ((*p1 < *win) ||
                (*p1 > *(win+2)) ||
                (*(p1+1) < *(win+1)) ||
                (*(p1+1) > *(win+3)))
                 return(-1);              /* Hela vektorn utanf�r f�nstret */


       /* Punkt 1 ligger innanf�r f�nstret, klipp punkt 2 om utanf�r. */ 

        if (*p2 < *win) {

            *(p2+1) += (*(p2+1) - *(p1+1))*(*win - *p2)/(*p2 - *p1);
            *p2 = *win;
            sts2 = 2;
        } else if (*p2 > *(win+2)) {

            *(p2+1) += (*(p2+1) - *(p1+1))*(*(win+2) - *p2)/(*p2 - *p1);
            *p2 = *(win+2);
            sts2 = 2;
        }

        if (*(p2+1) < *(win+1)) {

            *p2 += (*p2 - *p1)*(*(win+1) - *(p2+1))/(*(p2+1) - *(p1+1));
            *(p2+1) = *(win+1);
            sts2 = 2;
        } else if (*(p2+1) > *(win+3)) {

            *p2 += (*p2 - *p1)*(*(win+3) - *(p2+1))/(*(p2+1) - *(p1+1));
            *(p2+1) = *(win+3);
            sts2 = 2;
        }

        return(sts1 + sts2);
  }

/********************************************************/
