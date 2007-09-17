/*!*******************************************************
*
*    exdim.c
*    =======
*
*    EXeldm();     Create linear dimension
*    EXldim();     Create LDIM
*    EXecdm();     Create circular dimension
*    EXcdim();     Create CDIM
*    EXerdm();     Create radius dimension
*    EXrdim();     Create RDIM
*    EXeadm();     Create angular dimension
*    EXadim();     Create ADIM
*
*    This file is part of the VARKON Execute Library.
*    URL:  http://www.varkon.com
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"

extern DBptr   lsysla;
extern DBTmat *lsyspk;
extern DBTmat  lklsyi;

/*!******************************************************/

       short   EXeldm(
       DBId   *id,
       DBLdim *ldmptr,
       V2NAPA *pnp)

/*      Skapar l�ngdm�tt, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          ldmptr => Pekare till GM-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1442 = Kan ej lagra l�ngdm�tt i GM.
 *
 *      (C)microform ab 15/11 B.Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Add attributes.
*/
    ldmptr->hed_ld.blank = pnp->blank;
    ldmptr->hed_ld.pen   = pnp->pen;
    ldmptr->hed_ld.level = pnp->level;
    ldmptr->asiz_ld      = pnp->dasize;
    ldmptr->tsiz_ld      = pnp->dtsize;
    ldmptr->ndig_ld      = pnp->dndig;
    ldmptr->auto_ld      = pnp->dauto;
    ldmptr->wdt_ld       = pnp->width;
    ldmptr->pcsy_ld      = lsysla;
/*
***Insert in DB.
*/
    if ( pnp->save )
      {
      ldmptr->hed_ld.hit = pnp->hit;
      if ( DBinsert_ldim(ldmptr,id,&la) < 0 )
           return(erpush("EX1442",""));
      }
    else
      {
      ldmptr->hed_ld.hit = 0;
      }
/*
***Display.
*/
    if ( ldmptr->pcsy_ld > 0 ) DBread_csys(&csy,NULL,ldmptr->pcsy_ld);
    WPdrdm((DBAny *)ldmptr,&csy,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short     EXldim(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Skapar LDIM, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          p1     => Pekare till m�ttets startpunkt.
 *          p2     => Pekare till m�ttets slutpunkt.
 *          p3     => Pekare till textens l�ge.
 *          alt    => Alternativ, 0,1 eller 2.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1442 = Kan ej lagra l�ngdm�tt i GM.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXeldm, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBLdim   ldim;

/*
***Ldim geometry in local csys.
*/
    ldim.p1_ld.x_gm = p1->x_gm;
    ldim.p1_ld.y_gm = p1->y_gm;
    ldim.p1_ld.z_gm = 0.0;

    ldim.p2_ld.x_gm = p2->x_gm;
    ldim.p2_ld.y_gm = p2->y_gm;
    ldim.p2_ld.z_gm = 0.0;

    ldim.p3_ld.x_gm = p3->x_gm;
    ldim.p3_ld.y_gm = p3->y_gm;
    ldim.p3_ld.z_gm = 0.0;

    ldim.dtyp_ld = alt;
/*
***Save in DB and display.
*/
    return(EXeldm(id,&ldim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short   EXecdm(
       DBId   *id,
       DBCdim *cdmptr,
       V2NAPA *pnp)

/*      Skapar diameterm�tt, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          cdmptr => Pekare till GM-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1452 = Kan ej lagra diameterm�tt i GM.
 *
 *      (C)microform ab  15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Fyll i namnparameterdata.
*/
    cdmptr->hed_cd.blank = pnp->blank;
    cdmptr->hed_cd.pen   = pnp->pen;
    cdmptr->hed_cd.level = pnp->level;
    cdmptr->asiz_cd      = pnp->dasize;
    cdmptr->tsiz_cd      = pnp->dtsize;
    cdmptr->ndig_cd      = pnp->dndig;
    cdmptr->auto_cd      = pnp->dauto;
    cdmptr->wdt_cd       = pnp->width;
    cdmptr->pcsy_cd      = lsysla;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      cdmptr->hed_cd.hit = pnp->hit;
      if ( DBinsert_cdim(cdmptr,id,&la) < 0 )
             return(erpush("EX1452",""));
      }
    else
      {
      cdmptr->hed_cd.hit = 0;
      }
/*
***Rita.
*/
    if ( cdmptr->pcsy_cd > 0 ) DBread_csys(&csy,NULL,cdmptr->pcsy_cd);
    WPdrdm((DBAny *)cdmptr,&csy,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short      EXcdim(
       DBId     *id,
       DBId     *refid,
       DBVector *pos,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Skapar CDIM, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          refid  => Pekare till id f�r refererad cirkel.
 *          pos    => Pekare till textens l�ge.
 *          alt    => Alternativ, 0,1 eller 2.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1402 = Den refererade storhten finns ej i GM
 *              EX 1412 = Otill�ten geometri-typ f�r denna operation
 *              EX1452 = Kan ej lagra diameterm�tt i GM.
 *
 *      (C)microform ab  4/8/85 J. Kjellander
 *
 *      10/9/85  Nya felkoder, R. Svedin
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Uppdatering av referensr�knare, J. Kjellander
 *      20/11/85 Anrop till EXecdm, B. Doverud
 *      22/10/86 Ingen test av 3D/2D typ, R. Svedin
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    DBArc   oldarc;
    DBSeg   seg[4];
    DBCdim  cdim;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***H�mta la f�r den refererade cirkeln.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != ARCTYP )
         return(erpush("EX1412",""));
/*
***L�s cirkeldata.
*/
    DBread_arc(&oldarc,seg,la);
/*
***Ber�kna m�tt-data.
*/
    GE821(&oldarc,pos,alt,&cdim);
/*
***Lagra i gm och rita.
*/
    return(EXecdm(id,&cdim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short   EXerdm(
       DBId   *id,
       DBRdim *rdmptr,
       V2NAPA *pnp)

/*      Skapar radiem�tt, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          rdmptr => Pekare till GM-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1462 = Kan ej lagra radiem�tt i GM.
 *
 *      (C)microform ab  15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Fyll i namnparameterdata.
*/
    rdmptr->hed_rd.blank = pnp->blank;
    rdmptr->hed_rd.pen   = pnp->pen;
    rdmptr->hed_rd.level = pnp->level;
    rdmptr->asiz_rd      = pnp->dasize;
    rdmptr->tsiz_rd      = pnp->dtsize;
    rdmptr->ndig_rd      = pnp->dndig;
    rdmptr->auto_rd      = pnp->dauto;
    rdmptr->wdt_rd       = pnp->width;
    rdmptr->pcsy_rd      = lsysla;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      rdmptr->hed_rd.hit = pnp->hit;
      if ( DBinsert_rdim(rdmptr,id,&la) < 0 )
           return(erpush("EX1462",""));
      }
    else
      {
      rdmptr->hed_rd.hit = 0;
      }
/*
***Rita.
*/
    if ( rdmptr->pcsy_rd > 0 ) DBread_csys(&csy,NULL,rdmptr->pcsy_rd);
    WPdrdm((DBAny *)rdmptr,&csy,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short     EXrdim(
       DBId     *id,
       DBId     *refid,
       DBVector *p1,
       DBVector *p2,
       V2NAPA   *pnp)

/*      Skapar RDIM, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          refid  => Pekare till id f�r refererad cirkel.
 *          p1     => Pekare till m�ttets brytpunkt.
 *          p2     => Pekare till m�ttets slutpunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1402 = Den refererade storhten finns ej i GM
 *              EX1462 = Kan ej lagra radiem�tt i GM.
 *
 *      (C)microform ab  4/8/85 J. Kjellander
 *
 *      10/9/85  Nya felkoder, R. Svedin
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Uppdatering av referensr�knare, J. Kjellander
 *      20/11/85 Anrop til EXerdm, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *      29/4/87  3D-cirkel, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    DBArc   oldarc;
    DBSeg   seg[4];
    DBRdim  rdim;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      }
/*
***H�mta la f�r den refererade cirkeln.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != ARCTYP )
         return(erpush("EX1412",""));
/*
***L�s cirkeldata.
*/
    DBread_arc(&oldarc,seg,la);
/*
***Ber�kna m�tt-data.
*/
    GE822(&oldarc,p1,p2,&rdim);
/*
***Lagra i gm och rita.
*/
    return(EXerdm(id,&rdim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short   EXeadm(
       DBId   *id,
       DBAdim *admptr,
       V2NAPA *pnp)

/*      Skapar vinkelm�tt, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          admptr => Pekare till GM-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1472 = Kan ej lagra vinkelm�tt i GM.
 *
 *      (C)microform ab  15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Fyll i namnparameterdata.
*/
    admptr->hed_ad.blank = pnp->blank;
    admptr->hed_ad.pen   = pnp->pen;
    admptr->hed_ad.level = pnp->level;
    admptr->asiz_ad      = pnp->dasize;
    admptr->tsiz_ad      = pnp->dtsize;
    admptr->ndig_ad      = pnp->dndig;
    admptr->auto_ad      = pnp->dauto;
    admptr->wdt_ad       = pnp->width;
    admptr->pcsy_ad      = lsysla;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      admptr->hed_ad.hit = pnp->hit;
      if ( DBinsert_adim(admptr,id,&la) < 0 )
           return(erpush("EX1472",""));
      }
    else
      {
      admptr->hed_ad.hit = 0;
      }
/*
***Rita.
*/
    if ( admptr->pcsy_ad > 0 ) DBread_csys(&csy,NULL,admptr->pcsy_ad);
    WPdrdm((DBAny *)admptr,&csy,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short     EXadim(
       DBId     *id,
       DBId     *refid1,
       DBId     *refid2,
       DBVector *pos,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Skapar ADIM, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          refid  => Pekare till id f�r refererad linje-1.
 *          refid  => Pekare till id f�r refererad linje-2.
 *          pos    => Pekare till textens l�ge.
 *          alt    => Alternativ, + eller - 1,2,3 eller 4.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1402 = Den refererade storhten finns ej i GM
 *              EX1412 = Otill�ten geometri-typ f�r denna operation
 *              EX1532 = Kan ej ber�kna m�tt-data
 *              EX1472 = Kan ej lagra vinkelm�tt i GM.
 *
 *      (C)microform ab  4/8/85 J. Kjellander
 *
 *      10/9/85  Nya felkoder, R. Svedin
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Uppdatering av referensr�knare, J. Kjellander
 *      20/11/85 Anrop till EXeadm, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;
    DBLine   lin1,lin2;
    DBAdim   adim;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***H�mta la f�r den 1:a refererade linjen.
*/
    if ( DBget_pointer('I',refid1,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != LINTYP )
         return(erpush("EX1412",""));
/*
***L�s linjedata.
*/
    DBread_line(&lin1,la);
/*
***H�mta la f�r den 2:a refererade linjen.
*/
    if ( DBget_pointer('I',refid2,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != LINTYP )
         return(erpush("EX1412",""));
/*
***L�s linjedata.
*/
    DBread_line(&lin2,la);
/*
***Ber�kna m�tt-data.
*/
    if ( GE823(&lin1,&lin2,pos,alt,&adim) < 0 )
         return(erpush("EX1532",""));
/*
***Lagra i gm och rita.
*/
    return(EXeadm(id,&adim,pnp));
  }
/********************************************************/
