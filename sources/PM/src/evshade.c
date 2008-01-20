/**********************************************************************
*
*    evshade.c
*    =========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    evltvi();     Evaluerar CRE_LIGHT
*    evlton();     Evaluerar LIGHT_ON
*    evltof();     Evaluerar LIGHT_OFF
*    evcrco();     Evaluerar CRE_COLOR
*    evgtco();     Evaluerar GET_COLOR
*    evcrmt();     Evaluerar CRE_MATERIAL
*    evgtmt();     Evaluerar GET_MATERIAL
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
#include "../../WP/include/WP.h"

extern PMPARVA *proc_pv;  /* Access structure for MBS routines */
extern short    proc_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Ptr to function return value */

/********************************************************/

        short evltvi()

/*      Evaluates MBS procedure CRE_LIGHT().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   DBVector dir;
   DBfloat spotang,focus;

/*
***Only light number and light direction supplied.
*/
   if ( proc_pc == 2 )
     {
     dir.x_gm = 0.0;
     dir.y_gm = 0.0;
     dir.z_gm = 1.0;
     spotang  = 180.0;
     focus    = 0.0;
     }
/*
***Spot dir supplied. This means that par 2 is spot position.
*/
   else if ( proc_pc == 3 )
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = 180.0;
     focus    = 0.0;
     }
/*
***Spot angle supplied.
*/
   else if ( proc_pc == 4 )
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = proc_pv[4].par_va.lit.float_va;
     focus    = 0.0;
     }
/*
***Spot focus supplied.
*/
   else
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = proc_pv[4].par_va.lit.float_va;
     focus    = proc_pv[5].par_va.lit.float_va;
     }
/*
***Execute.
*/
   return(WPltvi(proc_pv[1].par_va.lit.int_va,
    (DBVector *)&proc_pv[2].par_va.lit.vec_va,
                &dir,spotang,focus));
 }

/********************************************************/
/********************************************************/

        short evlton()

/*      Evaluates MBS procedure LIGHT_ON().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   return(WPlton(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.float_va,
                 TRUE));
 }

/********************************************************/
/********************************************************/

        short evltof()

/*      Evaluates MBS procedure LIGHT_OFF().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   return(WPlton(proc_pv[1].par_va.lit.int_va,
                 100.0,
                 FALSE));
 }

/********************************************************/
/********************************************************/

        short evcrco()

/*      Evaluates MBS procedure CRE_COLOR().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   return(WPccol(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.int_va,
                 proc_pv[3].par_va.lit.int_va,
                 proc_pv[4].par_va.lit.int_va));
 }

/********************************************************/
/********************************************************/

        short evgtco()

/*      Evaluates MBS procedure GET_COLOR().
 *
 *      (C)2008-01-19 J. Kjellander
 *
 ********************************************************/

 {
   short   status;
   int     i,red,green,blue;
   bool    defined;
   PMLITVA litval[3];

/*
***Get the RGB values for this pen.
*/
   status = WPgpen(proc_pv[1].par_va.lit.int_va,&defined,&red,&green,&blue);
   if ( status < 0 ) return(status);
/*
***Is the pen defined ?
*/
   if ( defined )
     {
/*
***Yes, return 0 and copy RGB values to PMLITVA.
*/
     func_vp->lit.int_va = 0;

     litval[0].lit.int_va = red;
     litval[1].lit.int_va = green;
     litval[2].lit.int_va = blue;
/*
***Write to MBS variables.
*/
     for ( i=0; i<3; ++i )
       {
       inwvar(proc_pv[i+2].par_ty, proc_pv[i+2].par_va.lit.adr_va,
              0, NULL, &litval[i]);
       }
     }
/*
**This pen is not defined.
*/
   else func_vp->lit.int_va = -1;
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

        short evcrmt()

/*      Evaluates MBS procedure CRE_MATERIAL().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   return(WPcmat(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.int_va,
                 proc_pv[3].par_va.lit.int_va,
                 proc_pv[4].par_va.lit.int_va,
                 proc_pv[5].par_va.lit.int_va,
                 proc_pv[6].par_va.lit.int_va,
                 proc_pv[7].par_va.lit.int_va,
                 proc_pv[8].par_va.lit.int_va,
                 proc_pv[9].par_va.lit.int_va,
                 proc_pv[10].par_va.lit.int_va,
                 proc_pv[11].par_va.lit.int_va,
                 proc_pv[12].par_va.lit.int_va,
                 proc_pv[13].par_va.lit.int_va,
                 proc_pv[14].par_va.lit.int_va));
 }

/********************************************************/
/********************************************************/

        short evgtmt()

/*      Evaluates MBS procedure GET_MATERIAL().
 *
 *      (C)2008-01-19 J. Kjellander
 *
 ********************************************************/

 {
   short   status;
   int     i,ar,ag,ab,dr,dg,db,sr,sg,sb,er,eg,eb,s;
   bool    defined;
   PMLITVA litval[13];

/*
***Get the material values for this pen.
*/
   status = WPgmat(proc_pv[1].par_va.lit.int_va,&defined,&ar,&ag,&ab,
                   &dr,&dg,&db,&sr,&sg,&sb,&er,&eg,&eb,&s);
   if ( status < 0 ) return(status);
/*
***Is the pen defined ?
*/
   if ( defined )
     {
/*
***Yes, return 0 and copy material values to PMLITVA.
*/
     func_vp->lit.int_va = 0;

     litval[0].lit.int_va = ar;
     litval[1].lit.int_va = ag;
     litval[2].lit.int_va = ab;

     litval[3].lit.int_va = dr;
     litval[4].lit.int_va = dg;
     litval[5].lit.int_va = db;

     litval[6].lit.int_va = sr;
     litval[7].lit.int_va = sg;
     litval[8].lit.int_va = sb;

     litval[9].lit.int_va  = er;
     litval[10].lit.int_va = eg;
     litval[11].lit.int_va = eb;

     litval[12].lit.int_va = s;
/*
***Write to MBS variables.
*/
     for ( i=0; i<13; ++i )
       {
       inwvar(proc_pv[i+2].par_ty, proc_pv[i+2].par_va.lit.adr_va,
              0, NULL, &litval[i]);
       }
     }
/*
**This pen is not defined.
*/
   else func_vp->lit.int_va = -1;
/*
***The end.
*/
   return(0);
 }

/********************************************************/
