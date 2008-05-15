/**********************************************************************
*
*    evcsy.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    evcs3p();     Evaluerar CSYS_3P
*    evcs1p();     Evaluerar CSYS_1P
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
#include "../../EX/include/EX.h"

extern short  modtyp;

extern V2REFVA *geop_id; /* ingeop.c *identp  Storhetens ID */
extern PMPARVA *geop_pv; /* ingeop.c *pv Access structure for MBS routines */
extern short    geop_pc; /* ingeop.c parcount Number of actual parameters */
extern V2NAPA  *geop_np; /* ingeop.c *npblock Pekare till namnparameterblock.*/

/*!******************************************************/

        short    evcs3p()

/*      Evaluerar geometri-proceduren CSYS_3P.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parameterv�rden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      1996-05-28 Optionell Y-axel, J.Kjellander
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   DBVector *pyaxel;

/*
***Har vi f�tt en Y-axel eller har den utel�mnats.
*/
   if ( geop_pc ==  4 ) pyaxel = (DBVector *)&geop_pv[4].par_va.lit.vec_va;
   else            pyaxel =  NULL;
/*
***Exekvera CSYS_3P.
*/
   return(EXcs3p(  geop_id,   geop_pv[1].par_va.lit.str_va,
                 (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                 (DBVector *)&geop_pv[3].par_va.lit.vec_va,
                              pyaxel,
                              geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcs1p()

/*      Evaluerar geometri-proceduren CSYS_1P.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parameterv�rden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 30/9/87 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   DBfloat v1,v2,v3;

/*
***Exekvera CSYS_1P.
*/
   if ( modtyp == 3 )
     {
     if ( geop_pc == 2 )
       {
       v1 = v2 = v3 = 0.0;
       }
     else if ( geop_pc == 3 )
       {
       v1 = geop_pv[3].par_va.lit.float_va;
       v2 = v3 = 0.0;
       }
     else if ( geop_pc == 4 )
       {
       v1 = geop_pv[3].par_va.lit.float_va;
       v2 = geop_pv[4].par_va.lit.float_va;
       v3 = 0.0;
       }
     else
       {
       v1 = geop_pv[3].par_va.lit.float_va;
       v2 = geop_pv[4].par_va.lit.float_va;
       v3 = geop_pv[5].par_va.lit.float_va;
       }
     }
/*
***2D-fallet.
*/
   else
     {
     if ( geop_pc < 5 )
       {
       v1 = v2 = v3 = 0.0;
       }
     else
       {
       v3 = geop_pv[5].par_va.lit.float_va;
       v1 = v2 = 0.0;
       }
     }
/*
***Exekvera.
*/
   return(EXcs1p(   geop_id, geop_pv[1].par_va.lit.str_va,
                (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                             v1,v2,v3, geop_np));
}

/********************************************************/
/*!******************************************************/

        short    evcsud()

/*      Evaluates CSYS_USRDEF().
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parameterv�rden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      1996-05-28 Optionell Y-axel, J.Kjellander
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short  status;
   DBint   valadr;
   int     radsiz,fltsiz;
   PMLITVA fval;
   STTYTBL typtbl;
   STARRTY arrtyp;
   DBTmat  tr;
   
/*
***Ber�kna div. RTS-offset.
***radsiz = storleken p� en FLOAT (4)       Normalt 32  bytes.
***fltsiz = storleken p� en FLOAT           Normalt 8   bytes.
*/
   strtyp(geop_pv[2].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   radsiz = typtbl.size_ty;

   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Kopiera 4X4-matrisen till DBTmat.
*/
   valadr = geop_pv[2].par_va.lit.adr_va;

   ingval(valadr,arrtyp.base_arr,FALSE,&fval);
   tr.g11 = fval.lit.float_va;
   ingval(valadr+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g12 = fval.lit.float_va;
   ingval(valadr+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g13 = fval.lit.float_va;
   ingval(valadr+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g14 = fval.lit.float_va;

   ingval(valadr+radsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g21 = fval.lit.float_va;
   ingval(valadr+radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g22 = fval.lit.float_va;
   ingval(valadr+radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g23 = fval.lit.float_va;
   ingval(valadr+radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g24 = fval.lit.float_va;

   ingval(valadr+2*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g31 = fval.lit.float_va;
   ingval(valadr+2*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g32 = fval.lit.float_va;
   ingval(valadr+2*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g33 = fval.lit.float_va;
   ingval(valadr+2*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g34 = fval.lit.float_va;

   ingval(valadr+3*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g41 = fval.lit.float_va;
   ingval(valadr+3*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g42 = fval.lit.float_va;
   ingval(valadr+3*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g43 = fval.lit.float_va;
   ingval(valadr+3*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g44 = fval.lit.float_va;
   
   status = EXcsud(geop_id,
                   geop_pv[1].par_va.lit.str_va,
                  &tr,
                   geop_np);     

   return(status);
}

/********************************************************/