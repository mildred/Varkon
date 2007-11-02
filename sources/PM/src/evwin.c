/**********************************************************************
*
*    evwin.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    evcrwi();     Evaluerar CRE_WIN
*    evcred();     Evaluerar CRE_EDIT
*    evcrbu();     Evaluerar CRE_BUTTTON
*    evcrdb();     Evaluerar CRE_DBUTTTON
*    evcrfb();     Evaluerar CRE_FBUTTTON
*    evcric();     Evaluerar CRE_ICON
*    evcrfi();     Evaluerar CRE_FICON
*    evgted();     Evaluerar GET_EDIT
*    evuped();     Evaluerar UPD_EDIT
*    evgtbu();     Evaluerar GET_BUT
*    evgtwi();     Evaluerar GET_WIN
*    evshwi();     Evaluerar SHOW_WIN
*    evwtwi();     Evaluerar WAIT_WIN
*    evdlwi();     Evaluerar DEL_WIN
*    evgttl();     Evaluerar TEXTL_WIN
*    evgtth();     Evaluerar TEXTH_WIN
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../WP/include/WP.h"

extern PMPARVA *proc_pv;  /* inproc.c *pv Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern short    func_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Pekare till resultat. */

#ifdef UNIX
#include <X11/Xlib.h>
#endif

#ifdef UNIX

/*!******************************************************/

        short evcrwi()

/*      Evaluerar CRE_WIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:  return - PM1052 = Ok�nd f�nstertyp.    
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   if ( func_pv[5].par_va.lit.int_va == 0 )
     {
     return(WPwciw((short)func_pv[1].par_va.lit.vec_va.x_val,
                   (short)func_pv[1].par_va.lit.vec_va.y_val,
                   (short)func_pv[2].par_va.lit.float_va,
                   (short)func_pv[3].par_va.lit.float_va,
                          func_pv[4].par_va.lit.str_va,
                         &func_vp->lit.int_va));
     }
   else if ( func_pv[5].par_va.lit.int_va == 1 )
     {
     return(WPwcgw((short)func_pv[1].par_va.lit.vec_va.x_val,
                   (short)func_pv[1].par_va.lit.vec_va.y_val,
                   (short)func_pv[2].par_va.lit.float_va,
                   (short)func_pv[3].par_va.lit.float_va,
                          func_pv[4].par_va.lit.str_va,
                          FALSE,
                         &func_vp->lit.int_va));
     }
   else return(erpush("PM1052",""));

 }

/********************************************************/
/*!******************************************************/

        short evcred()

/*      Evaluerar CRE_EDIT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 3/12/93 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(WPmced(       func_pv[1].par_va.lit.int_va,
                 (short)func_pv[2].par_va.lit.vec_va.x_val,
                 (short)func_pv[2].par_va.lit.vec_va.y_val,
                 (short)func_pv[3].par_va.lit.float_va,
                 (short)func_pv[4].par_va.lit.float_va,
                 (short)func_pv[5].par_va.lit.float_va,
                        func_pv[6].par_va.lit.str_va,
                 (short)func_pv[7].par_va.lit.int_va,
                       &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcrbu()

/*      Evaluerar CRE_BUTTON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *      2007-10-23 Button types, J.Kjellander
 *
 ******************************************************!*/

{
   char   font[V3STRLEN+1];
   short  cb,cf;

/*
***N�n s�rskild font ? Parameter nummer 8.
***Om inte skicka tom str�ng.
*/
   if ( func_pc > 7 ) strcpy(font,func_pv[8].par_va.lit.str_va);
   else               font[0] = '\0';
/*
***Bakgrundsf�rg ? Parameter nummer 9.
*/
   if ( func_pc > 8 ) cb = (short)func_pv[9].par_va.lit.int_va;
   else
     {
     if ( func_pv[5].par_va.lit.float_va == 0.0 ) cb = WP_BGND1;
     else                                         cb = WP_BGND2;
     }
/*
***F�rgrundsf�rg ? Parameter nummer 10.
*/
   if ( func_pc > 9 ) cf = (short)func_pv[10].par_va.lit.int_va;
   else               cf = WP_FGND;
/*
***If border width = 0, create a LABEL, otherwise
***create a TOGGLEBUTTON.
*/
   if ( func_pv[5].par_va.lit.float_va == 0.0 )
     {
     return(WPcrlb(       func_pv[1].par_va.lit.int_va,
                   (short)func_pv[2].par_va.lit.vec_va.x_val,
                   (short)func_pv[2].par_va.lit.vec_va.y_val,
                   (short)func_pv[3].par_va.lit.float_va,
                   (short)func_pv[4].par_va.lit.float_va,
                          func_pv[6].par_va.lit.str_va,
                         &func_vp->lit.int_va));
      }
   else
     {
     return(WPcrsb(       func_pv[1].par_va.lit.int_va,
                   (short)func_pv[2].par_va.lit.vec_va.x_val,
                   (short)func_pv[2].par_va.lit.vec_va.y_val,
                   (short)func_pv[3].par_va.lit.float_va,
                   (short)func_pv[4].par_va.lit.float_va,
                   (short)func_pv[5].par_va.lit.float_va,
                          func_pv[6].par_va.lit.str_va,
                          func_pv[7].par_va.lit.str_va,
                          font,
                          cb,cf,
                         &func_vp->lit.int_va));
     }
}

/********************************************************/
/*!******************************************************/

        short evcrdb()

/*      Evaluerar CRE_DBUTTON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1996-12-09 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   char font[V3STRLEN+1];
   int  cb,cf;

/*
***N�n s�rskild font ? Parameter nummer 8.
***Om inte skicka tom str�ng.
*/
   if ( func_pc > 6 ) strcpy(font,func_pv[7].par_va.lit.str_va);
   else            font[0] = '\0';
/*
***Bakgrundsf�rg ? Parameter nummer 8.
***Om inte skicka 6.
*/
   if ( func_pc > 7 ) cb = func_pv[8].par_va.lit.int_va;
   else            cb = 6;
/*
***F�rgrundsf�rg ? Parameter nummer 9.
***Om inte skicka 1.
*/
   if ( func_pc > 8 ) cf = func_pv[9].par_va.lit.int_va;
   else            cf = 1;
/*
***Skapa knappen.
*/
   return(WPcrsb(     func_pv[1].par_va.lit.int_va,
                 (int)func_pv[2].par_va.lit.vec_va.x_val,
                 (int)func_pv[2].par_va.lit.vec_va.y_val,
                 (int)func_pv[3].par_va.lit.float_va,
                 (int)func_pv[4].par_va.lit.float_va,
                 (int)func_pv[5].par_va.lit.float_va,
                      func_pv[6].par_va.lit.str_va,
                      func_pv[6].par_va.lit.str_va,
                      font,
                      cb,cf,
                     &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcrfb()

/*      Evaluerar CRE_FBUTTON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1996-05-20 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{

/*
***Skapa knappen.
*/
   return(WPcrfb(       func_pv[1].par_va.lit.int_va,
                 (short)func_pv[2].par_va.lit.vec_va.x_val,
                 (short)func_pv[2].par_va.lit.vec_va.y_val,
                 (short)func_pv[3].par_va.lit.float_va,
                 (short)func_pv[4].par_va.lit.float_va,
                        func_pv[5].par_va.lit.str_va,
                        func_pv[6].par_va.lit.str_va,
                 (short)func_pv[7].par_va.lit.int_va,
                       &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcric()

/*      Evaluerar CRE_ICON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 12/1/94 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short  cb,cf;

/*
***Bakgrundsf�rg ? Parameter nummer 5.
***Om inte skicka 6.
*/
   if ( func_pc > 4 ) cb = (short)func_pv[5].par_va.lit.int_va;
   else            cb = 6;
/*
***F�rgrundsf�rg ? Parameter nummer 6.
***Om inte skicka 1.
*/
   if ( func_pc > 5 ) cf = (short)func_pv[6].par_va.lit.int_va;
   else            cf = 1;
/*
***Skapa ikonen.
*/
   return(WPmcic(       func_pv[1].par_va.lit.int_va,
                 (short)func_pv[2].par_va.lit.vec_va.x_val,
                 (short)func_pv[2].par_va.lit.vec_va.y_val,
                 (short)func_pv[3].par_va.lit.float_va,
                        func_pv[4].par_va.lit.str_va,
                        cb,cf,
                       &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcrfi()

/*      Evaluerar CRE_FICON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1996-05-20 J.Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
/*
***Skapa ikonen.
*/
   return(WPcrfi(       func_pv[1].par_va.lit.int_va,
                 (short)func_pv[2].par_va.lit.vec_va.x_val,
                 (short)func_pv[2].par_va.lit.vec_va.y_val,
                        func_pv[3].par_va.lit.str_va,
                        func_pv[4].par_va.lit.str_va,
                 (short)func_pv[5].par_va.lit.int_va,
                       &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evgted()

/*      Evaluerar GET_EDIT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(WPgted(func_pv[1].par_va.lit.int_va,
                 func_pv[2].par_va.lit.int_va,
                 func_vp->lit.str_va));
}

/********************************************************/
/*!******************************************************/

        short evuped()

/*      Evaluerar UPD_EDIT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)2007-02-02 J.Kjellander
 *
 ******************************************************!*/

{
   return(WPuped(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.int_va,
                 proc_pv[3].par_va.lit.str_va));
}

/********************************************************/
/*!******************************************************/

        short evgtbu()

/*      Evaluerar GET_BUTTON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(WPgtbu(func_pv[1].par_va.lit.int_va,
                 func_pv[2].par_va.lit.int_va,
                &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evgtwi()

/*      Evaluerar GET_WIN.
 *
 *      In: extern pv   => Pekare till array med parameterv�rden
 *          extern pc   => Antal parametrar.
 *
 *      Ut: *valp = Funktionsv�rde ifyllt.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1996-05-21 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short   status;
   int     x,y,dx,dy,typ;
   char    rubrik[V3STRLEN+1];
   PMLITVA litval[5];

   status = WPgtwi(proc_pv[1].par_va.lit.int_va,&x,&y,&dx,&dy,&typ,rubrik);
   if ( status < 0 ) return(status);
/*
***Kopiera parameterv�rden till PMLITVA.
*/
     litval[0].lit.vec_va.x_val = x;
     litval[0].lit.vec_va.y_val = y;
     litval[0].lit.vec_va.z_val = 0;
     litval[1].lit.int_va       = dx;
     litval[2].lit.int_va       = dy;

     if ( proc_pc > 4 ) litval[3].lit.int_va = typ;
     if ( proc_pc > 5 ) strcpy(litval[4].lit.str_va,rubrik);
/*
***Skriv parameterv�rden till motsvarande MBS-variabler.
*/
     return(evwval(litval,proc_pc-1,proc_pv));
}

/********************************************************/
/*!******************************************************/

        short evshwi()

/*      Evaluerar SHOW_WIN.
 *
 *      In: extern pv   => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(WPwshw(proc_pv[1].par_va.lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evwtwi()

/*      Evaluerar WAIT_WIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2001-04-20 Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(WPwwtw(func_pv[1].par_va.lit.int_va,
                 SLEVEL_MBS,
                &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evdlwi()

/*      Evaluerar DEL_WIN.
 *
 *      In: extern pv   => Pekare till array med parameterv�rden
 *          extern pc   => Antal parametrar
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{

/*
***�r det ett huvudf�nster (npar = 1) eller ett subf�nster
***(npar = 2) ?
*/
   if ( proc_pc == 1 )
     return(WPwdel(proc_pv[1].par_va.lit.int_va));
   else
     return(WPwdls(proc_pv[1].par_va.lit.int_va,
                   proc_pv[2].par_va.lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evgttl()

/*      Evaluerar TEXTL_WIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1996-05-21 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short status;
   int   npix;
   char  fntnam[V3STRLEN+1];

/*
***Om font angetts tar vi den annars skickar vi tom str�ng.
*/
   if ( func_pc == 1 ) strcpy(fntnam,"");
   else           strcpy(fntnam,func_pv[2].par_va.lit.str_va);
/*
***H�mta textl�ngd.
*/
   status = WPgtsl(func_pv[1].par_va.lit.str_va,fntnam,&npix);
   if ( status < 0 ) return(status);

   func_vp->lit.int_va = npix;

   return(0);
}

/********************************************************/
/*!******************************************************/

        short evgtth()

/*      Evaluerar TEXTH_WIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1996-05-21 J. Kjellander
 *
 *      2001-04-20 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short status;
   int   npix;
   char  fntnam[V3STRLEN+1];

/*
***Om font angetts tar vi den annars skickar vi tom str�ng.
*/
   if ( func_pc == 0 ) strcpy(fntnam,"");
   else           strcpy(fntnam,func_pv[1].par_va.lit.str_va);
/*
***H�mta textl�ngd.
*/
   status = WPgtsh(fntnam,&npix);
   if ( status < 0 ) return(status);

   func_vp->lit.int_va = npix;

   return(0);
}

/********************************************************/

#else
#ifdef WIN32

/*!******************************************************/

        short evcrwi()


/*      Evaluerar CRE_WIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:  return - PM1052 = Ok�nd f�nstertyp.    
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   if ( func_pv[5].par_va.lit.int_va == 0 )
     {
     return(mswciw((int)func_pv[1].par_va.lit.vec_va.x_val,
                   (int)func_pv[1].par_va.lit.vec_va.y_val,
                   (int)func_pv[2].par_va.lit.float_va,
                   (int)func_pv[3].par_va.lit.float_va,
                        func_pv[4].par_va.lit.str_va,
                  &func_vp->lit.int_va));
     }
   else if ( func_pv[5].par_va.lit.int_va == 1 )
     {
     return(mswcgw((int)func_pv[1].par_va.lit.vec_va.x_val,
                   (int)func_pv[1].par_va.lit.vec_va.y_val,
                   (int)func_pv[2].par_va.lit.float_va,
                   (int)func_pv[3].par_va.lit.float_va,
                        func_pv[4].par_va.lit.str_va,
                        FALSE,
                       &func_vp->lit.int_va));
     }
   else return(erpush("PM1052",""));
}

/********************************************************/
/*!******************************************************/

        short evcred()

/*      Evaluerar CRE_EDIT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(msmced(     func_pv[1].par_va.lit.int_va,
                 (int)func_pv[2].par_va.lit.vec_va.x_val,
                 (int)func_pv[2].par_va.lit.vec_va.y_val,
                 (int)func_pv[3].par_va.lit.float_va,
                 (int)func_pv[4].par_va.lit.float_va,
                 (int)func_pv[5].par_va.lit.float_va,
                      func_pv[6].par_va.lit.str_va,
                 (int)func_pv[7].par_va.lit.int_va,
                     &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcrfb()


/*      Evaluerar CRE_FBUTTON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{

/*
***Skapa knappen.
*/
   return(mscrfb(     func_pv[1].par_va.lit.int_va,
                 (int)func_pv[2].par_va.lit.vec_va.x_val,
                 (int)func_pv[2].par_va.lit.vec_va.y_val,
                 (int)func_pv[3].par_va.lit.float_va,
                 (int)func_pv[4].par_va.lit.float_va,
                      func_pv[5].par_va.lit.str_va,
                      func_pv[6].par_va.lit.str_va,
                      func_pv[7].par_va.lit.int_va,
                     &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcrbu()

/*      Evaluerar CRE_BUTTON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   char font[V3STRLEN+1];
   int  cb,cf;

/*
***N�n s�rskild font ? Parameter nummer 8.
***Om inte skicka tom str�ng.
*/
   if ( func_pc > 7 ) strcpy(font,func_pv[8].par_va.lit.str_va);
   else            font[0] = '\0';
/*
***Bakgrundsf�rg ? Parameter nummer 9.
***Om inte skicka 6.
*/
   if ( func_pc > 8 ) cb = func_pv[9].par_va.lit.int_va;
   else            cb = 6;
/*
***F�rgrundsf�rg ? Parameter nummer 10.
***Om inte skicka 1.
*/
   if ( func_pc > 9 ) cf = func_pv[10].par_va.lit.int_va;
   else            cf = 1;
/*
***Skapa knappen.
*/
   return(msmcbu(     func_pv[1].par_va.lit.int_va,
                 (int)func_pv[2].par_va.lit.vec_va.x_val,
                 (int)func_pv[2].par_va.lit.vec_va.y_val,
                 (int)func_pv[3].par_va.lit.float_va,
                 (int)func_pv[4].par_va.lit.float_va,
                 (int)func_pv[5].par_va.lit.float_va,
                      func_pv[6].par_va.lit.str_va,
                      func_pv[7].par_va.lit.str_va,
                      font,
                      cb,cf,
                     &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcrdb()


/*      Evaluerar CRE_DBUTTON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   char font[V3STRLEN+1];
   int  cb,cf;

/*
***N�n s�rskild font ? Parameter nummer 8.
***Om inte skicka tom str�ng.
*/
   if ( func_pc > 6 ) strcpy(font,func_pv[7].par_va.lit.str_va);
   else            font[0] = '\0';
/*
***Bakgrundsf�rg ? Parameter nummer 8.
***Om inte skicka 6.
*/
   if ( func_pc > 7 ) cb = func_pv[8].par_va.lit.int_va;
   else            cb = 6;
/*
***F�rgrundsf�rg ? Parameter nummer 9.
***Om inte skicka 1.
*/
   if ( func_pc > 8 ) cf = func_pv[9].par_va.lit.int_va;
   else            cf = 1;
/*
***Skapa knappen.
*/
   return(mscrdb (     func_pv[1].par_va.lit.int_va,
                 (int)func_pv[2].par_va.lit.vec_va.x_val,
                 (int)func_pv[2].par_va.lit.vec_va.y_val,
                 (int)func_pv[3].par_va.lit.float_va,
                 (int)func_pv[4].par_va.lit.float_va,
                 (int)func_pv[5].par_va.lit.float_va,
                      func_pv[6].par_va.lit.str_va,
                      font,
                      cb,cf,
                     &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcric()


/*      Evaluerar CRE_ICON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   int cb,cf;

/*
***Bakgrundsf�rg ? Parameter nummer 5.
***Om inte skicka 6.
*/
   if ( func_pc > 4 ) cb = func_pv[5].par_va.lit.int_va;
   else            cb = 6;
/*
***F�rgrundsf�rg ? Parameter nummer 6.
***Om inte skicka 1.
*/
   if ( func_pc > 5 ) cf = func_pv[6].par_va.lit.int_va;
   else            cf = 1;
/*
***Skapa ikonen.
*/
   return(msmcic(     func_pv[1].par_va.lit.int_va,
                 (int)func_pv[2].par_va.lit.vec_va.x_val,
                 (int)func_pv[2].par_va.lit.vec_va.y_val,
                 (int)func_pv[3].par_va.lit.float_va,
                      func_pv[4].par_va.lit.str_va,
                      cb,cf,
                     &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evcrfi()


/*      Evaluerar CRE_FICON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 96-05-20 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
/*
***Skapa ikonen.
*/
   return(mscrfi(     func_pv[1].par_va.lit.int_va,
                 (int)func_pv[2].par_va.lit.vec_va.x_val,
                 (int)func_pv[2].par_va.lit.vec_va.y_val,
                      func_pv[3].par_va.lit.str_va,
                      func_pv[4].par_va.lit.str_va,
                      func_pv[5].par_va.lit.int_va,
                     &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evgted()

/*      Evaluerar GET_EDIT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(msgted(func_pv[1].par_va.lit.int_va,
                 func_pv[2].par_va.lit.int_va,
                 func_vp->lit.str_va));
}

/********************************************************/
/*!******************************************************/

        short evgtbu()

/*      Evaluerar GET_BUTTON.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(msgtbu(func_pv[1].par_va.lit.int_va,
                 func_pv[2].par_va.lit.int_va,
                &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evgtwi()

/*      Evaluerar GET_WIN.
 *
 *      In: extern pv   => Pekare till array med parameterv�rden
 *          extern pc   => Antal parametrar.
 *
 *      Ut: *valp = Funktionsv�rde ifyllt.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1996-05-21 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short   status;
   int     x,y,dx,dy,typ;
   char    rubrik[V3STRLEN+1];
   PMLITVA litval[5];

   status = msgtwi(proc_pv[1].par_va.lit.int_va,&x,&y,&dx,&dy,
	               &typ,rubrik);
   if ( status < 0 ) return(status);
/*
***Kopiera parameterv�rden till PMLITVA.
*/
     litval[0].lit.vec_va.x_val = x;
     litval[0].lit.vec_va.y_val = y;
     litval[0].lit.vec_va.z_val = 0;
     litval[1].lit.int_va       = dx;
     litval[2].lit.int_va       = dy;

     if ( proc_pc > 4 ) litval[3].lit.int_va = typ;
     if ( proc_pc > 5 ) strcpy(litval[4].lit.str_va,rubrik);
/*
***Skriv parameterv�rden till motsvarande MBS-variabler.
*/
     return(evwval(litval,(short)(proc_pc-1),proc_pv));
}

/********************************************************/
/*!******************************************************/

        short evshwi()

/*      Evaluerar SHOW_WIN.
 *
 *      In: extern pv   => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(mswshw(proc_pv[1].par_va.lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evwtwi()

/*      Evaluerar WAIT_WIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-03-06 Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(mswwtw(  func_pv[1].par_va.lit.int_va,
                   SLEVEL_MBS,
                  &func_vp->lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evdlwi()

/*      Evaluerar DEL_WIN.
 *
 *      In: extern pv   => Pekare till array med parameterv�rden
 *          extern pc   => Antal parametrar
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{

/*
***�r det ett huvudf�nster (npar = 1) eller ett subf�nster
***(npar = 2) ?
*/
   if ( proc_pc == 1 )
     return(mswdel(proc_pv[1].par_va.lit.int_va));
   else
     return(mswdls(proc_pv[1].par_va.lit.int_va,
                   proc_pv[2].par_va.lit.int_va));
}

/********************************************************/
/*!******************************************************/

        short evgttl()

/*      Evaluerar TEXTL_WIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1996-05-21 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short status;
   int   npix;
   char  fntnam[V3STRLEN+1];

/*
***Om font angetts tar vi den annars skickar vi tom str�ng.
*/
   if ( func_pc == 1 ) strcpy(fntnam,"");
   else           strcpy(fntnam,func_pv[2].par_va.lit.str_va);
/*
***H�mta textl�ngd.
*/
   status = msgtsl(func_pv[1].par_va.lit.str_va,fntnam,&npix);
   if ( status < 0 ) return(status);

   func_vp->lit.int_va = npix;

   return(0);
}

/********************************************************/
/*!******************************************************/

        short evgtth()

/*      Evaluerar TEXTH_WIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1996-05-21 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short status;
   int   npix;
   char  fntnam[V3STRLEN+1];

/*
***Om font angetts tar vi den annars skickar vi tom str�ng.
*/
   if ( func_pc == 0 ) strcpy(fntnam,"");
   else           strcpy(fntnam,func_pv[1].par_va.lit.str_va);
/*
***H�mta textl�ngd.
*/
   status = msgtsh(fntnam,&npix);
   if ( status < 0 ) return(status);

   func_vp->lit.int_va = npix;

   return(0);
}

/********************************************************/

#else
short evcrwi(){;};
short evcred(){;};
short evcrbu(){;};
short evcric(){;};
short evgted(){;};
short evgtbu(){;};
short evshwi(){;};
short evwtwi(){;};
short evdlwi(){;};
short evcrfb(){;};
short evcrfi(){;};
short evgtwi(){;};
short evgttl(){;};
short evgtth(){;};
#endif
#endif

/********************************************************/
