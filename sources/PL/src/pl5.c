/*!******************************************************************/
/*  File: pl5.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   pntpoi();    Paint point                                       */
/*   pntlin();    Paint line                                        */
/*   pntarc();    Paint arc                                         */
/*   pnttxt();    Paint text                                        */
/*   pntply();    Paint polyline                                    */
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

extern double skala,penwdt,actwdt,doffs;
extern double ptabp[];
extern int    npaint;          /* Antal m�lningar */
extern bool   arccon;          /* M�la cirklars rand */

int     actpen = -1;
double x[PLYMXV],y[PLYMXV],dx[PLYMXV],dy[PLYMXV];
char   a[PLYMXV];

#define VISIBLE 1
#define TOL1    0.001

/*!*****************************************************/

        short pntpoi(GMPOI *poipek)

/*      M�lar en bred punkt. Globala variabler npaint,
 *      doffs,penwdt och actwdt s�tts av plcwdt().
 *
 *      In:
 *         poipek  =  Adress till punkt-gmstruktur.
 *
 *      (c)microform ab 6/2/91 J. Kjellander
 *
 ******************************************************!*/

 {
   short  n;

/*
***�r r�tt penna (eller m�lningsm�nster) aktiv ?.
*/
   if ( poipek->hed_p.pen != actpen )
     {
     actpen = poipek->hed_p.pen;
     plcwdt(actpen);
     }
/*
***Skapa polyline och rita med pntply().
*/
   x[0] = poipek->crd_p.x_gm;
   y[0] = poipek->crd_p.y_gm;
   n = -1;
   plbpoi(poipek,&n,x,y,a);
   pntply(n,x,y,a);

   return(0);
 }

/*!*****************************************************/
/*!*****************************************************/

        short pntlin(GMLIN *linpek)

/*      M�lar en bred linje. Globala variabler npaint,
 *      doffs,penwdt och actwdt s�tts av plcwdt().
 *
 *      In:
 *         linpek  =  Adress till linje-gmstruktur.
 *
 *      (c)microform ab 6/2/91 J. Kjellander
 *
 ******************************************************!*/

 {
   double dx,dy,dl,offs;
   int    i;
   short  n;

/*
***�r r�tt penna (eller m�lningsm�nster) aktiv ?.
*/
   if ( linpek->hed_l.pen != actpen )
     {
     actpen = linpek->hed_l.pen;
     plcwdt(actpen);
     }
/*
***Om npaint > 1 skall m�lning utf�ras.
***Ber�kna normaliserad riktningsvektor f�r offset (dx,dy).
*/
   if ( npaint > 1 )
     {
     dx = linpek->crd2_l.y_gm - linpek->crd1_l.y_gm;
     dy = linpek->crd1_l.x_gm - linpek->crd2_l.x_gm;
     dl = SQRT(dx*dx+dy*dy);
     dx /= dl;
     dy /= dl;
/*
***1:a linjens offset.
*/
     offs = (actwdt - penwdt)/2.0;

     for ( i=0; i<npaint; ++i )
       {
       x[0] = linpek->crd1_l.x_gm + (offs-i*doffs)*dx/skala;
       y[0] = linpek->crd1_l.y_gm + (offs-i*doffs)*dy/skala;
       x[1] = linpek->crd2_l.x_gm + (offs-i*doffs)*dx/skala;
       y[1] = linpek->crd2_l.y_gm + (offs-i*doffs)*dy/skala;
       n = -1;
       plblin(linpek,&n,x,y,a);
       plpoly(n,x,y,a);
       }
     }
/*
***M�lning  �r inte aktuellt.
*/
   else
     {
     x[0] = linpek->crd1_l.x_gm;
     y[0] = linpek->crd1_l.y_gm;
     x[1] = linpek->crd2_l.x_gm;
     y[1] = linpek->crd2_l.y_gm;
     n = -1;
     plblin(linpek,&n,x,y,a);
     plpoly(n,x,y,a);
     }

   return(0);
   
 }

/*******************************************************/
/*!*****************************************************/

        short pntarc(GMARC *arcpek)

/*      M�lar en bred arc. Globala variabler npaint,
 *      doffs,penwdt och actwdt s�tts av plcwdt().
 *
 *      In:
 *         arcpek  =  Adress till arc-gmstruktur.
 *
 *      (c)microform ab 7/2/91 J. Kjellander
 *
 *      18/2(91 Extra m�lning av rand, J. Kjellander
 *
 ******************************************************!*/

 {
   double offs,orgrad,minwdt;
   int    i;
   short  n,minpen=0;

/*
***�r r�tt penna (eller m�lningsm�nster) aktiv ?.
*/
   if ( arcpek->hed_a.pen != actpen )
     {
     actpen = arcpek->hed_a.pen;
     plcwdt(actpen);
     }
/*
***Om npaint > 1 skall m�lning utf�ras.
*/
   if ( npaint > 1 )
     {
/*
***Heldragen arc g�rs r�tt, streckad som polyline.
*/
     if ( arcpek->fnt_a == 0 )
       {
       offs = (actwdt - penwdt)/2.0;
       orgrad = arcpek->r_a;
       arcpek->r_a += offs/skala;

       for ( i=0; i<npaint; ++i )
         {
         n = -1;
         plbarc(arcpek,&n,x,y,a);
         plpoly(n,x,y,a);
         arcpek->r_a -= doffs/skala;
         }
/*
***Om cirklars kontur skall fyllas i, g�r det.
***Leta upp smalaste pennan och dra en extra b�ge
***l�ngs vardera konturen.
*/
       if ( arccon )
         {
         minwdt = 1E10;
         for ( i=0; i<WPNMAX; ++i )
           if ( ptabp[i] > 0.0  &&  ptabp[i] < minwdt )
             {
             minwdt = ptabp[i];
             minpen = i;
             }
         plchpn(minpen);
         actpen = -1;
         arcpek->r_a = orgrad + actwdt/2.0 - minwdt/2.0;
         n = -1;
         plbarc(arcpek,&n,x,y,a);
         plpoly(n,x,y,a);
         arcpek->r_a = orgrad - actwdt/2.0 + minwdt/2.0;
         n = -1;
         plbarc(arcpek,&n,x,y,a);
         plpoly(n,x,y,a);
         }
       }
     else
       {
       n = -1;
       plbarc(arcpek,&n,x,y,a);
       pntply(n,x,y,a);
       }
     }
/*
***M�lning  �r inte aktuellt.
*/
   else
     {
     n = -1;
     plbarc(arcpek,&n,x,y,a);
     plpoly(n,x,y,a);
     }

   return(0);
   
 }

/********************************************************/
/*!*****************************************************/

        short pnttxt(
        GMTXT  *txtpek,
        char   *str)

/*      M�lar en bred punkt. Globala variabler npaint,
 *      doffs,penwdt och actwdt s�tts av plcwdt().
 *
 *      In:
 *         txtpek  =  Adress till text-gmstruktur.
 *         str     =  Text att plotta.
 *
 *      (c)microform ab 6/2/91 J. Kjellander
 *
 ******************************************************!*/

 {
   short  n;

/*
***�r r�tt penna (eller m�lningsm�nster) aktiv ?.
*/
   if ( txtpek->hed_tx.pen != actpen )
     {
     actpen = txtpek->hed_tx.pen;
     plcwdt(actpen);
     }
/*
***Skapa polyline och rita med pntply().
*/
   x[0] = txtpek->crd_tx.x_gm;
   y[0] = txtpek->crd_tx.y_gm;
   n = -1;
   plbtxt(txtpek,str,&n,x,y,a);
   pntply(n,x,y,a);

   return(0);
 }

/*!*****************************************************/
/*!*****************************************************/

        short pntply(
        short  n,
        double x[],
        double y[],
        char   a[])

/*      M�lar en bred polylinje. Globala variabler npaint,
 *      doffs,penwdt och actwdt s�tts av plcwdt(). actpen
 *      s�tts av anropande rutiner.
 *
 *      In:
 *         n   = Offset till vektorslut.
 *         x,y = Koordinater.
 *         a   = T�nd/sl�ck.
 *
 *      (c)microform ab 7/2/91 J. Kjellander
 *
 *       26/11/91 Div. med 0, J. Kjellander
 *
 ******************************************************!*/

 {
   double dl,offs,dx1,dy1;
   int    i,j;

/*
***Om npaint > 1 skall m�lning utf�ras.
*/
   if ( npaint > 1 )
     {
/*
***Ber�kna normaliserad riktningsvektor f�r offset i
***startpunkten....
*/
     dx[0] = y[1] - y[0]; 
     dy[0] = x[0] - x[1];
     dl = SQRT(dx[0]*dx[0]+dy[0]*dy[0]);
     if ( dl > TOL1 )
       {
       dx[0] /= dl;
       dy[0] /= dl;
       }
/*
***Alla brytpunkter.... Om vektorn f�re och efter en brytpunkt
***b�da �r t�nda ber�knas en gemensam offset-riktning.
*/
     for ( i=1; i<n; ++i )
       {
       if ( ((a[i]&VISIBLE) == VISIBLE)  &&  ((a[i+1]&VISIBLE) == VISIBLE) )
         {
         dx1 = y[i+1] - y[i]; 
         dy1 = x[i] - x[i+1];
         dl = SQRT(dx1*dx1+dy1*dy1);
         if ( dl > TOL1 )
           {
           dx1 /= dl;
           dy1 /= dl;
           }
         dx[i] = (dx[i-1] + dx1)/2.0;
         dy[i] = (dy[i-1] + dy1)/2.0;
         dl = SQRT(dx[i]*dx[i]+dy[i]*dy[i]);
         if ( dl > TOL1 )
           {
           dx[i] /= dl;
           dy[i] /= dl;
           }
         }
/*
***Om vektorn f�re brytpunkten �r t�nd men vektorn efter
***brytpunkten �r sl�ckt, �r detta sista vektorn i ett
***t�nt t�g. V�lj vinkelr�t offset-riktning i �nden.
***
*/
       else if ( ((a[i]&VISIBLE) == VISIBLE)  &&
                 ((a[i+1]&VISIBLE) != VISIBLE) )
         {
         dx[i] = y[i] - y[i-1]; 
         dy[i] = x[i-1] - x[i];
         dl = SQRT(dx[i]*dx[i]+dy[i]*dy[i]);
         if ( dl > TOL1 )
           {
           dx[i] /= dl;
           dy[i] /= dl;
           }
         }
       else
         {
         dx[i] = y[i+1] - y[i]; 
         dy[i] = x[i] - x[i+1];
         dl = SQRT(dx[i]*dx[i]+dy[i]*dy[i]);
         if ( dl > TOL1 )
           {
           dx[i] /= dl;
           dy[i] /= dl;
           }
         }
       }
/*
***Samt slutpunkten.
*/
     dx[n] = y[n] - y[n-1]; 
     dy[n] = x[n-1] - x[n];
     dl = SQRT(dx[n]*dx[n]+dy[n]*dy[n]);
     if ( dl > TOL1 )
       {
       dx[n] /= dl;
       dy[n] /= dl;
       }
/*
***1:a polylinjens offset.
*/
     offs = (actwdt - penwdt)/2.0;

     for ( j=0; j<n+1; ++j )
       {
       x[j] = x[j] + offs*dx[j]/skala;
       y[j] = y[j] + offs*dy[j]/skala;
       }
/*
***M�la.
*/
     for ( i=0; i<npaint; ++i )
       {
       plpoly(n,x,y,a);
       for ( j=0; j<n+1; ++j )
         {
         x[j] = x[j] - doffs*dx[j]/skala;
         y[j] = y[j] - doffs*dy[j]/skala;
         }
       }
     }
/*
***M�lning  �r inte aktuellt.
*/
   else plpoly(n,x,y,a);

   return(0);
 }

/*!*****************************************************/
