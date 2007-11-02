/*!*****************************************************
*
*    exdia1.c 
*    ========
*
*    EXpspt();      Interface routine for PSH_PMT
*    EXpopt();      Interface routine for POP_PMT
*    EXcrts();      Interface routine for CRE_TSTR
*    EXgtts();      Interface routine for GET_TSTR
*    EXcrmy();      Interface routine for CRE_MEN
*    EXpsmy();      Interface routine for PSH_MEN
*    EXpomy();      Interface routine for POP_MEN
*    EXgtal();      Interface routine for GET_ALT
*    EXgtmy();      Interface routine for GET_MEN
*    EXlsin();      Interface routine for LST_INI
*    EXlsex();      Interface routine for LST_EXI
*    EXlsln();      Interface routine for LST_LIN
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://varkon.sourceforge.net
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"
#include "../../WP/include/WP.h"

extern char    txtmem []; /* Textstr�ngar */
extern char   *txtind[];  /* Pekare till t-str�ngar */
extern MNUALT  malist[];  /* Menyalternativ */  
extern MNUDAT  mnutab[];  /* Menyer */
extern char   *fstmem;    /* Pekare till n�sta lediga t-str�ng */

/*!******************************************************/

        short EXpspt(char *pstr)
 
/*      Interface routine for PSH_PMT().
 *
 *      In: *pstr => C ptr to prompt string.
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      19/4/87  Kodning, J. Kjellander
 *      2007-09-22 WPaddmess_mcwin(), J.Kjellander
 *
 ******************************************************!*/

  {
    WPaddmess_mcwin(pstr,WP_MESSAGE);

    return(0);
  }

/********************************************************/
/********************************************************/

        short EXpopt()
 
/*      Interface routine for POP_PMT(). Clears
 *      message line.
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      19/4/87  Kodning, J. Kjellander
 *      2007-09-22 WPclear_mcwin(), J.Kjellander
 *
 ******************************************************!*/

  {
    WPclear_mcwin();

    return(0);
  }

/********************************************************/
/********************************************************/

        short EXcrts(
        DBshort tsnr,
        char   *tstr)
 
/*      Interface-rutin f�r CRE_TSTR. Skapar/uppdaterar
 *      en t-str�ng med visst nummer.
 *
 *      In:  tsnr  => T-str�ngens nummer.
 *           *tstr => Pekare till t-str�ng.
 *
 *      FV:  0     => Ok.
 *
 *      Felkoder: EX1792 = Otill�tet tsnr
 *                EX1802 = txtmem fullt.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      19/4/87    Kodning, J. Kjellander
 *      1996-05-28 �teranv�ndning av minne, J.Kjellander
 *
 ******************************************************!*/

  {
    char txtbuf[V3STRLEN+1],errbuf[10];

/*
***Kolla att t-str�ngnumret ligger inom till�tet intervall.
*/
    if ( tsnr < 0 || tsnr >= TXTMAX )
      {
      sprintf(errbuf,"%d",tsnr);
      return(erpush("EX1792",errbuf));
      }
/*
***Kopiera och mappa "%num" till kontrolltecken.
*/
    IGatoc(txtbuf,tstr);
/*
***Om t-str�ngen med detta nummer har skapats redan tidigare
***och det nu �r fr�gan om att er�tta den med n�got annat 
***och detta "n�got annat" inte tar mer plats �n f�rut kan
***vi �teranv�nda samma plats.
*/
    if ( txtind[tsnr] != NULL  &&
         (strlen(txtind[tsnr])  >= strlen(txtbuf)) )
      {
      strcpy(txtind[tsnr],txtbuf);
      }
/*
***Om t-str�ngen inte �r skapad sen tidigare eller nu �r
***l�ngre �n tidigare tar vi plats fr�n txtmem.
***Kolla is�fall f�rst att det finns plats.
*/
    else
      {
      if ( fstmem > txtmem+CHRMAX-81 ) return(erpush("EX1802",""));
/*
***Lagra t-str�ngen i txtmem.
*/
      txtind[tsnr] = fstmem;
      strcpy(fstmem,txtbuf);
      fstmem += strlen(txtbuf) +1;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtts(
        DBshort  tsnr,
        PMLITVA *tstr)
 
/*      Interface-rutin f�r GET_TSTR. Returnerar t-str�ng
 *      med visst nummer.
 *
 *      In:  tsnr  => T-str�ngens nummer.
 *
 *      Ut:  *tstr => Pekare till t-str�ng.
 *
 *      FV:  0     => Ok.
 *
 *      Felkoder:  EX1792 = Otill�tet tsnr
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      19/4/87  Kodning, J. Kjellander
 *      24/7/87  Feltest, R. Svedin
 *
 ******************************************************!*/

  {

    char errbuf[10];

/*
***Kolla att t-str�ngnumret inom till�tet intervall.
*/
    if ( tsnr < 0 || tsnr >= TXTMAX )
      {
      sprintf(errbuf,"%d",tsnr);
      return(erpush("EX1792",errbuf));
      }

    if ( txtind[tsnr] == NULL ) strcpy(tstr->lit.str_va,"");
    else strcpy(tstr->lit.str_va,txtind[tsnr]);

    return(0);
  }

/********************************************************/
/********************************************************/

        short EXcrmy(
        PMPARVA *pv,
        DBshort  npar)
 
/*      Interface-rutin f�r CRE_MEN. Skapar en meny.
 *
 *      In: pv   = Pekare till rubrik och alternativ
 *          npar = Antal alternativ 
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      Felkoder: EX1812 = Otill�tet menynummer.
 *                EX1822 = Can't malloc()
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      19/4/87  Kodning, J. Kjellander
 *      27/2/91  Ny minneshanterare, J. Kjellander
 *
 ******************************************************!*/

  {
    short mnum,i,nalt;
    char  rubrik[V3STRLEN+1];
    char  altstr[20][V3STRLEN+1];
    char  alttyp[20];
    short altnum[20];
    char *actstr;
    char  errbuf[10];

/*
***Kolla att menynummer ligger inom till�tet intervall.
*/
    mnum = (short)pv[1].par_va.lit.int_va;

    if ( mnum < 0 || mnum >= MNUMAX )
      {
      sprintf(errbuf,"%d",mnum);
      return(erpush("EX1812",errbuf));
      }
/*
***Rubrik.
*/
    IGatoc(rubrik,pv[2].par_va.lit.str_va);
/*
***Alternativ.
*/
    nalt = (short)((npar-2)/2);

    for ( i=0; i<nalt; ++i )
      {
      IGatoc(altstr[i],pv[3+2*i].par_va.lit.str_va);
      actstr = pv[4+2*i].par_va.lit.str_va;
      alttyp[i] = *actstr;
      sscanf(actstr+1,"%hd",&altnum[i]);
      }
/*
***Lagra.
*/
   if ( IGstmu(mnum,rubrik,nalt,altstr,alttyp,altnum) < 0 )
      {
      sprintf(errbuf,"%d",mnum);
      return(erpush("EX1822",errbuf));
      }
 
   return(0);
  }

/********************************************************/
/********************************************************/

        short EXpsmy(DBshort mnr)
 
/*      Interface-rutin f�r PSH_MEN. Skriver ut en meny
 *      i meny-arean, menyn blir aktiv meny.
 *
 *      In:  mnr   => Menyns nummer.
 *
 *      Ut:  Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      20/4/87 Kodning, J. Kjellander
 *      16/1/96 WIN32, J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef WIN32
    msshmu((int)mnr);
#else
    IGaamu(mnr);
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXpomy()
 
/*      Interface-rutin f�r POP_MEN. Stryker aktiv meny
 *      och g�r f�reg�ende meny aktiv.
 *
 *      In:  Inget.
 *
 *      Ut:  Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      20/4/87 Kodning, J. Kjellander
 *      16/1/96 WIN32, J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef WIN32
    mshdmu();
#else
    IGsamu();
#endif

    return(0);
  }

/********************************************************/
/********************************************************/

        short EXgtal(
        char    *prstr,
        PMLITVA *akod)
 
/*      Interface-rutin f�r GET_ALT. Promptar anv�ndaren att
 *      g�ra ett val ur en meny.
 *
 *      In: *prstr => Pekare till promptstr�ng.
 *
 *      Ut: *akod  => Pekare till aktionskod.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      20/4/87 Kodning, J. Kjellander
 *      1/3/94  Snabbval, J. Kjellander
 *
 ******************************************************!*/

  {
    short  alttyp;
    MNUALT *pmualt;

    IGplma(prstr,IG_MESS);

    IGgalt(&pmualt,&alttyp);

    if ( pmualt == NULL ) strcpy(akod->lit.str_va,"");
    else
      {
      akod->lit.str_va[0] = pmualt->acttyp;
      sprintf(&(akod->lit.str_va[1]),"%d",pmualt->actnum);
      }

    IGrsma();

    return(0);
  }

/********************************************************/
/********************************************************/

        short EXgtmy(
        DBshort  mnr,
        char    *rubr)
 
/*      Interface-rutin f�r GET_MEN. Returnerar rubrik-
 *      text f�r angiven meny.
 *
 *      In:  mnr   => Menynummer.
 *
 *      Ut: *rubr  => Pekare till rubriktext.
 *
 *      FV:  0     => Ok.
 *
 *      Felkoder:  EX1812 = Otill�tet menynummer
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      20/4/87  Kodning, J. Kjellander
 *      24/4/87  Feltest, R. Svedin
 *
 ******************************************************!*/

  {
    char errbuf[10];

/*
***Kolla att meny-numret ligger inom till�tet intervall.
*/
    if ( mnr < 0 || mnr >= MNUMAX )
      {
      sprintf(errbuf,"%d",mnr);
      return(erpush("EX1812",errbuf));
      }

    if ( mnutab[mnr].rubr != NULL ) strcpy(rubr,mnutab[mnr].rubr);
    else strcpy(rubr,"");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXlsin(char *rubr)

/*      Interface-rutin f�r LST_INI. Initierar list-
 *      arean.
 *
 *      In: *rubr  => Pekare till rubrik.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 ******************************************************!*/

  {
    WPinla(rubr);
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXlsex()

/*      Interface-rutin f�r LST_EXI. Avslutar list-
 *      arean.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 ******************************************************!*/

  {
    WPexla(TRUE);
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXlsln(char *utstr)

/*      Interface-rutin f�r LST_LIN. Skriver en rad
 *      i list-arean.
 *
 *      In: *utstr => Pekare till str�ng.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 ******************************************************!*/

  {
    WPalla(utstr,(short)1 );
    return(0);
  }

/********************************************************/
