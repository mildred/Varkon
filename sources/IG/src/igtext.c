/*!******************************************************************/
/*  igtext.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGtext();   Generate text... statement                          */
/*  IGctxv();   Edit text angle                                     */
/*  IGctxs();   Edit text string                                    */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"

extern short   v3mode;

/*!******************************************************/

       short IGtext()

/*      Genererar text...sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TEXT sats
 *
 *      (C)microform ab 15/4/85 J. Kjellander
 *
 *      3/7/85   Felhantering, B. Doverud
 *      4/9/85   Anrop till IGcges(), B. Doverud
 *      16/11/85 t-str�ng, J. Kjellander
 *      6/3/86   Defaultstr�ng, J. Kjellander
 *      20/3/86  Anrop pmtcon, B. Doverud
 *      23/3/86  IGcpos(pnr, B. Doverud
 *      24/3/86  Felutg�ng, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam;
    pm_ptr  retla,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3;
    char    istr[V3STRLEN+1];

    static char sdstr[V3STRLEN+1] = "";
    static char vstr[V3STRLEN+1] = "0.0";

/*
***Skapa position.
*/
start:
    if ( (status=IGcpos(264,&exnpt1)) < 0 ) goto end;
/*
***Skapa vridning.
*/
    if ( (status=IGcflt(17,vstr,istr,&exnpt2)) < 0 ) goto end;
    strcpy(vstr,istr);
/*
***Skapa str�ng.
*/
    if ( (status=IGcstr(10,sdstr,istr,&exnpt3)) < 0 ) goto end;
    strcpy(sdstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("TEXT",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);
/*
***Felutg�ngar.
*/
error:
    erpush("IG5023","");
    errmes();
    WPerhg();
    goto start;
  }

/********************************************************/
/*!******************************************************/

        short IGctxv()

/*      �ndra texts vinkel.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      Felkoder: IG3502 = Storheten ing�r i en part
 *
 *      (C)microform ab 23/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    DBfloat   tv;
    bool    end,right;
    char    str[V3STRLEN+1];
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    DBId    idvek[MXINIV];
    DBText   txt;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda p� textens ID.
*/
loop:
    IGptma(268,IG_MESS);
    typ = TXTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    IGrsma();
/*
***Kolla att storheten inte ing�r i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***L�s nuvarande vinkel ur GM.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,str,la);
    tv = txt.v_tx;
/*
***Fr�ga efter ny vinkel.
*/
    sprintf(strbuf,"%s%g  %s",IGgtts(43),tv,IGgtts(248));
    IGplma(strbuf,IG_INP);

    if ( (status=IGcflv(0,istr,dstr,&tv)) < 0 ) goto exit;

    strcpy(dstr,istr);
    IGrsma();
/*
***�ndra vinkel i GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    txt.v_tx = tv;
    DBupdate_text(&txt,str,la);
    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    IGrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGctxs()

/*      �ndra texts text.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      Felkoder: IG3502 = Storheten ing�r i en part
 *
 *      (C)microform ab 23/3/88 J. Kjellander
 *
 *      19/3/89 Default=gamla texten, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    char    str[V3STRLEN+1];
    char    dstr[V3STRLEN+1];
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    bool    end,right;
    DBId    idvek[MXINIV];
    DBText   txt;

/*
***Ta reda p� textens ID.
*/
loop:
    IGptma(268,IG_MESS);
    typ = TXTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    IGrsma();
/*
***Kolla att storheten inte ing�r i en part.
*/
    if ( v3mode & BAS_MOD  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***L�s nuvarande text ur GM.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,str,la);
/*
***Fr�ga efter ny text.
*/
    sprintf(strbuf,"%s%s  %s",IGgtts(43),str,IGgtts(248));
    IGplma(strbuf,IG_INP);
    strcpy(dstr,str);
    if ( (status=IGcstv(0,istr,dstr,str)) < 0 ) goto exit;
    IGrsma();
/*
***�ndra text i GM och GP.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    DBupdate_text(&txt,str,la);
    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Om igen.
*/
    goto loop;
/*
***Avslutning.
*/
exit:
    WPerhg();
    IGrsma();
    return(status);
  }

/********************************************************/
