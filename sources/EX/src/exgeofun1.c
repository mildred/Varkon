/*!*****************************************************
*
*    exgeofun1.c
*    ===========
*
*    EXstrt();    Interface routine for START
*    EXend();     Interface routine for END
*    EXon();      Interface routine for ON
*    EXion();     Interface routine for INV_ON
*    EXtang();    Interface routine for TANG
*    EXitan();    Interface routine for INV_TANG
*    EXcurv();    Interface routine for CURV
*    EXicur();    Interface routine for INV_CURV
*    EXcen();     Interface routine for CENTRE
*    EXnorm();    Interface routine for NORM
*    EXarcl();    Interface routine for ARCL
*    EXiarc();    Interface routine for INV_ARCL
*    EXsuar();    Interface routine for SURFACE_AREA
*    EXsear();    Interface routine for SECTION_AREA
*    EXsecg();    Interface routine for SECTION_CGRAV
*    EXtxtl();    Interface routine for TEXTL
*    EXsect();    Interface routine for INTERSECT
*    EXnsec();    Interface routine for N_INTERSECT
*    EXidnt();    Interface routine for IDENT
*    EXpos();     Interface routine for POS
*    EXscr();     Interface routine for SCREEN
*    EXarea();    Interface routine for AREA,CGRAV
*    EXpinc();    Interface routine for PTS_IN_CONE
*
*    EXusec();    Used by EXsect()
*
*    This file is part of the VARKON Execute Library.
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
*    (C)1984-2005, Johan Kjellander, �rebro university
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
/*#include "../../GP/include/GP.h"*/

#ifdef UNIX
#include "../../WP/include/WP.h"
#endif

#include "../include/EX.h"
#include <string.h>

#ifdef UNIX
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <process.h>
#endif

extern bool    tmpref;
extern DBptr   msysla,lsysla;
extern DBTmat *lsyspk,*msyspk;
extern DBTmat  modsys,lklsys,lklsyi;
extern V2NAPA  defnap;

/*!******************************************************/

        short EXstrt(
        DBId     *idpek,
        DBVector *vecptr)

/*      Interface-rutin f�r START().
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 1997-01-25 J. Kjellander
 *
 ******************************************************!*/

  {
    DBLine  lin;
    DBArc  arc;
    DBCurve  cur;
    DBSeg *segpek;
    DBSeg  arcseg[4];
    DBptr  la;
    DBetype  typ;
    short  status;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&lin, la);
      status=GEposition((DBAny *)&lin,NULL,0.0,0.0,vecptr);
      break;

      case ARCTYP:
      DBread_arc(&arc,arcseg,la);
      status=GEposition((DBAny *)&arc,(char *)arcseg,0.0,0.0,vecptr);
      break;

      case CURTYP:
      DBread_curve(&cur,NULL,&segpek,la);
      status=GEposition((DBAny *)&cur,(char *)segpek,0.0,0.0,vecptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","START()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXend(
        DBId     *idpek,
        DBVector *vecptr)

/*      Interface-rutin f�r END().
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 1997-01-23 J. Kjellander
 *
 ******************************************************!*/

  {
    DBLine  lin;
    DBArc  arc;
    DBCurve  cur;
    DBSeg *segpek;
    DBSeg  arcseg[4];
    DBptr  la;
    DBetype  typ;
    short  status;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&lin, la);
      status=GEposition((DBAny *)&lin,NULL,1.0,0.0,vecptr);
      break;

      case ARCTYP:
      DBread_arc(&arc,arcseg,la);
      status=GEposition((DBAny *)&arc,(char *)arcseg,1.0,0.0,vecptr);
      break;

      case CURTYP:
      DBread_curve(&cur,NULL,&segpek,la);
      status=GEposition((DBAny *)&cur,(char *)segpek,
                        (DBfloat)cur.ns_cu,0.0,vecptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","END()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXon(
        DBId     *idpek,
        DBfloat   u,
        DBfloat   v,
        DBVector *vecptr)

/*      Interface-rutin f�r funktionen ON. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GEposition() f�r att ber�kna positionen.
 *      Uppdaterar den refererade storhetens referens-
 *      r�knare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          u,v    => Parameterv�rden.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 11/1/85 J. Kjellander
 *
 *      29/12/85   Symbol, J. Kjellander
 *      29/10/86   v3dbuf, J. Kjellander
 *      20/12/91   B�gl-par. 3D-cirklar, J. Kjellander
 *      22/2/93    Ytor, J. Kjellander
 *      7/6/93     Dynamisk allokering av segment, J. Kjellander
 *      21/3/94    Nya DBPatch, J. Kjellander
 *      11/10/95   gmrdpat1(), J. Kjellander
 *       9/12/95   sur209, G  Liden
 *      1997-12-17 sur209(), J.Kjellander
 *      1998-09-24 b_plan, J.Kjellander
 *      1999-12-18 sur209->varkon_sur_eval_gm, G  Liden
 *      2007-09-24 3D dims, J.Kjellander
 *
 ******************************************************!*/

  {
    DBAny  gmpost;
    DBSeg *segpek;
    DBSeg  arcseg[4];
    DBCsys csy;
    EVALS  xyz;
    DBptr  la;
    DBetype  typ;
    short  status;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case POITYP:
      DBread_point(&gmpost.poi_un, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case LINTYP:
      DBread_line(&gmpost.lin_un, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status=GEposition(&gmpost,(char *)arcseg,u,v,vecptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEposition(&gmpost,(char *)segpek,u,v,vecptr);
      DBfree_segments(segpek);
      break;

      case SURTYP:
      DBread_surface(&gmpost.sur_un, la);
      if ( (status=varkon_sur_eval_gm(
             (DBSurf*)&gmpost,(DBint)0, u,v,&xyz)) < 0 ) return(status);
      vecptr->x_gm = xyz.r_x;
      vecptr->y_gm = xyz.r_y;
      vecptr->z_gm = xyz.r_z;
      break;

      case BPLTYP:
      DBread_bplane(&gmpost.bpl_un,la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case CSYTYP:
      DBread_csys(&gmpost.csy_un, NULL, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case TXTTYP:
      DBread_text(&gmpost.txt_un, NULL, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case LDMTYP:
      DBread_ldim(&gmpost.ldm_un, &csy, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      if ( gmpost.ldm_un.pcsy_ld > 0 ) GEtfpos_to_basic(vecptr,&csy.mat_pl,vecptr);
      break;

      case CDMTYP:
      DBread_cdim(&gmpost.cdm_un, &csy, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      if ( gmpost.cdm_un.pcsy_cd > 0 ) GEtfpos_to_basic(vecptr,&csy.mat_pl,vecptr);
      break;

      case RDMTYP:
      DBread_rdim(&gmpost.rdm_un, &csy, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      if ( gmpost.rdm_un.pcsy_rd > 0 ) GEtfpos_to_basic(vecptr,&csy.mat_pl,vecptr);
      break;

      case ADMTYP:
      DBread_adim(&gmpost.adm_un, &csy, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      if ( gmpost.adm_un.pcsy_ad > 0 ) GEtfpos_to_basic(vecptr,&csy.mat_pl,vecptr);
      break;

      default:
      return(erpush("EX1412","ON()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXion(
        DBId     *idpek,
        DBVector *vecptr,
        DBshort   tnr,
        DBfloat  *tptr)

/*      Interface-rutin f�r funktionen INV_ON. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GE703() f�r att ber�kna t-v�rdet.
 *      Uppdaterar den refererade storhetens referens-
 *      r�knare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          vecptr => Pekare till position.
 *          tnr    => Positionens ordningsnummer.
 *
 *      Ut: *tptr => t-v�rde.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 12/11/91 J. Kjellander
 *
 *      7/6/93 Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg *segpek;
    DBSeg  arcseg[4];

/*
***Transformera till BASIC.
*/
   if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,&lklsyi,vecptr);
/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
***Ber�kna u-v�rde. Observera att GE703() returnerar relativ
***b�gl�ngd 0<t<1 f�r linjer och cirklar (�ven 3D-cirklar) men
***inte kurvor. F�r kurvor returneras global parameter 0<t<nseg.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un, la);
      status = GE703(&gmpost,NULL,vecptr,tnr,tptr);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status = GE703(&gmpost,arcseg,vecptr,tnr,tptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status = GE703(&gmpost,segpek,vecptr,tnr,tptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","INV_ON()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXtang(
        DBId     *idpek,
        DBfloat   t,
        DBTmat   *crdptr,
        DBVector *vecptr)

/*      Interface-rutin f�r funktionen TANG. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GEtangent() f�r att ber�kna tangenten.
 *      Uppdaterar den refererade storhetens referens-
 *      r�knare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          t      => Parameterv�rde.
 *          crdptr => Pekare till lokalt koordinatsystem.
 *
 *      Ut: *vecptr => Normaliserad tangentvector.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 3/1/86 J. Kjellander
 *
 *      29/10/86 v3dbuf, J. Kjellander
 *      20/12/91 B�gl-par. 3D-cirklar, J. Kjellander
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg *segpek;
    DBSeg  arcseg[4];
    DBTmat pmat;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un, la);
      status=GEtangent(&gmpost,NULL,t,NULL,vecptr);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg, la);
      status=GEtangent(&gmpost,arcseg,t,NULL,vecptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEtangent(&gmpost,segpek,t,NULL,vecptr);
      DBfree_segments(segpek);
      break;

      case CSYTYP:
      DBread_csys(&gmpost.csy_un, &pmat, la);
      status=GEtangent(&gmpost,NULL,t,&pmat,vecptr);
      break;

      default:
      return(erpush("EX1412","TANG()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfvec_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXitan(
        DBId     *idpek,
        DBVector *vecptr,
        DBshort   tnr,
        DBfloat  *tptr)

/*      Interface-rutin f�r funktionen INV_TANG. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GE702() f�r att ber�kna t-v�rdet.
 *      Uppdaterar den refererade storhetens referens-
 *      r�knare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          vecptr => Pekare till tangentvector.
 *          tnr    => Tangentens ordningsnummer.
 *
 *      Ut: *tptr => t-v�rde.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 12/11/91 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    short   status;
    DBAny   gmpost;
    DBSeg  *segpek;
    DBSeg   arcseg[4];

/*
***Transformera tangenten till BASIC.
*/
   if ( lsyspk != NULL ) GEtfvec_to_local(vecptr,&lklsyi,vecptr);
/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata. Ber�kna u-v�rde.
*/
    switch (typ)
      {
      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status = GE702(&gmpost,arcseg,vecptr,tnr,tptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status = GE702(&gmpost,segpek,vecptr,tnr,tptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","INV_TANG()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    EXcurv(
        DBId    *idpek,
        DBfloat  t,
        DBfloat *fltptr)

/*      Interface-rutin f�r funktionen CURV. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GEcurvature() f�r att ber�kna kr�kninen.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          t      => Parameterv�rde.
 *          fltptr => Pekare till utdata.
 *
 *      Ut: *fltptr => Kr�kningscentrum.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 12/12/91 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg *segpek;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ, l�s geometridata och ber�kna kr�kning.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un,la);
      status=GEcurvature(&gmpost,NULL,t,fltptr);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un, NULL, la);
      status=GEcurvature(&gmpost,NULL,t,fltptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEcurvature(&gmpost,segpek,t,fltptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","CURV()"));
      }
/*
***Slut.
*/
    if ( status < 0 ) return(status);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    EXicur(
        DBId    *idpek,
        DBfloat  kappa,
        DBshort  tnr,
        DBfloat *tptr)

/*      Interface-rutin f�r funktionen INV_CURV. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GE704() f�r att ber�kna t-v�rdet.
 *      Uppdaterar den refererade storhetens referens-
 *      r�knare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          kappa  => �nskad kr�kning.
 *          tnr    => Kr�kningens ordningsnummer.
 *
 *      Ut: *tptr => t-v�rde.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 19/11/91 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBfloat tmp;
    DBCurve  cur;
    DBSeg *segpek;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ, l�s geometridata och ber�kna u-v�rde.
*/
    switch (typ)
      {
      case CURTYP:
      DBread_curve(&cur,NULL,&segpek, la);
      tmp = kappa;
      status = GE704((DBAny *)&cur,segpek,&tmp,tnr,tptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","INV_CURV()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXcen(
        DBId     *idpek,
        DBfloat   t,
        DBTmat   *crdptr,
        DBVector *vecptr)

/*      Interface-rutin f�r funktionen CENTRE. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GEcentre() f�r att ber�kna kr�knings-
 *      centrum. Uppdaterar den refererade storhetens
 *      referensr�knare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          t      => Parameterv�rde.
 *          crdptr => Pekare till lokalt koordinatsystem.
 *          vecptr => Pekare till utdata.
 *
 *      Ut: *vecptr => Kr�kningscentrum.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 3/1/86 J. Kjellander
 *
 *      29/10/86 v3dbuf, J. Kjellander
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg *segpek;
    DBSeg  arcseg[4];

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      if ( gmpost.arc_un.ns_a > 1 ) t = t*gmpost.arc_un.ns_a;
      status=GEcentre(&gmpost,arcseg,t,vecptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEcentre(&gmpost,segpek,t,vecptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","CENTRE()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXnorm(
        DBId     *idpek,
        DBfloat   u,
        DBfloat   v,
        DBVector *vecptr)

/*      Interface-rutin f�r funktionen NORM.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          u,v    => Parameterv�rden.
 *          vecptr => Pekare till resultat.
 *
 *      Ut: *vecptr => Normaliserad normalvector.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 3/12/92 J. Kjellander
 *
 *      22/2/93    Ytor, J. Kjellander
 *      7/6/93     Dynamisk allokering av segment, J. Kjellander
 *      21/3/94    Nya DBPatch, J. Kjellander
 *       9/12/95   sur209, G  Liden      
 *      1997-12-17 sur209(), J.Kjellander
 *      1999-12-18 sur209->varkon_sur_eval_gm, G  Liden
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    EVALS  xyz;
    DBAny  gmpost;
    DBSeg  arcseg[4];
    DBSeg *segpek;
    DBTmat pmat;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status=GEnormal(&gmpost,(char *)arcseg,u,v,vecptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEnormal(&gmpost,(char *)segpek,u,v,vecptr);
      DBfree_segments(segpek);
      break;

      case CSYTYP:
      DBread_csys(&gmpost.csy_un, &pmat, la);
      status=GEnormal(&gmpost,(char *)&pmat,u,v,vecptr);
      break;

      case BPLTYP:
      DBread_bplane(&gmpost.bpl_un, la);
      status=GEnormal(&gmpost,NULL,u,v,vecptr);
      break;

      case SURTYP:
      DBread_surface(&gmpost.sur_un, la);
      if ( (status=varkon_sur_eval_gm((DBSurf*)&gmpost,(DBint)3, 
                 u,v,&xyz)) < 0 ) return(status);
      vecptr->x_gm = xyz.n_x;
      vecptr->y_gm = xyz.n_y;
      vecptr->z_gm = xyz.n_z;
      break;

      default:
      return(erpush("EX1412","NORM()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfvec_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    EXarcl(
        DBId    *idpek,
        DBfloat *length)

/*      Interface-rutin f�r funktionen ARCL. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur GM och anropar
 *      geo-rutinen GEarclength() f�r att ber�kna b�gl�ngden.
 *      Uppdaterar den refererade storhetens referens-
 *      r�knare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *
 *      Ut: *length => B�gl�ngd.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 3/1/86 J. Kjellander
 *
 *      29/10/86 v3dbuf, J. Kjellander
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *      23/11/94 EX2062, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg  arcseg[4];
    DBSeg *segpek;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un, la);
      status=GEarclength(&gmpost,NULL,length);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status=GEarclength(&gmpost,arcseg,length);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      if ( (*length=gmpost.cur_un.al_cu) == 0.0 )
        status=GEarclength(&gmpost,segpek,length);
      else status = 0;
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","ARCL()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(erpush("EX2062","ARCL"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    EXiarc(
        DBId    *idpek,
        DBfloat  l,
        DBfloat *tptr)

/*      Interface-rutin f�r funktionen INV_ARCL. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GE717() f�r att ber�kna t-v�rdet.
 *      Uppdaterar den refererade storhetens referens-
 *      r�knare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          l      => L�ngd i mm.
 *
 *      Ut: *tptr => t-v�rde.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 12/12/91 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *      23/11/94 EX2062, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBfloat  relu,length;
    DBAny  gmpost;
    DBSeg  arcseg[4];
    DBSeg *segpek;

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un,la);
      status = GEarclength(&gmpost,NULL,&length);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status = GEarclength(&gmpost,arcseg,&length);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      if ( (length=gmpost.cur_un.al_cu) == 0.0 )
        status = GEarclength(&gmpost,segpek,&length);
      else
        status = 0;
      break;

      default:
      return(erpush("EX1412","INV_ARCL()"));
      }
/*
***Gick det bra ?
*/
   if ( status < 0 ) return(status);
/*
***Indata �r en l�ngd i millimeter.
***Ber�kna relativt b�gl�ngd.
*/
   relu = l/length;
  *tptr = relu;
/*
***F�r en 3D-cirkel skall relativ b�gl�ngd returneras. Detta �r
***redan klart. F�r en kurva skall global parameter returneras.
***Observera att DBSeg-minne deallokeras f�rst h�r.
*/
   if ( typ == CURTYP )
     {
     status = GE717(&gmpost,segpek,lsyspk,relu,tptr);
    *tptr -= 1.0;
     DBfree_segments(segpek);
     }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(erpush("EX2062","INV_ARCL"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    EXsuar(
        DBId    *idpek,
        DBfloat  tol,
        DBfloat *p_area)

/*      Interface-rutin f�r funktionen SURFACE_AREA().
 *
 *      In: idpek  => Pekare till ytans identitet.
 *          tol    => Tolerens.
 *          p_area => Pekare till utdata.
 *
 *      Ut: *p_area => Ber�knad area.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *       1997-12-18 sur300 tillagd G. Liden
 *       1999-12-18 sur300->varkon_sur_sarea  G. Liden
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBSurf sur;
    DBPatch *patpek;
    DBTmat csys;    /* Coordinate system for m.o.i.            */
    DBint  acc;     /* Calculation accuracy:                   */
                    /* Eq. 1: Whole patch                      */
                    /* Eq. 2: Patch divided  4 times           */
                    /* Eq. 3: Patch divided  9 times           */
                    /* Eq. 4:  .....                           */
    DBint  c_case;  /* Calculation case:                       */
                    /* Eq. 1: ..                               */
                    /* Eq. 2: ..                               */
                    /* Eq. 3: ..                               */
    DBfloat  sarea;   /* Surface area                            */
    DBVector  cog;     /* Center of gravity                       */
    DBVector  moi;     /* Moments of inertia (jx,jy,jz)           */
                    /* w.r.t to coordinate system p_csys       */
    DBVector  axis1;   /* Principal axis 1                        */
    DBVector  axis2;   /* Principal axis 2                        */
    DBVector  axis3;   /* Principal axis 3                        */
/*                                                                  */
/*                                                                  */

/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch (typ)
      {
      case SURTYP:
      DBread_surface(&sur,la);
      DBread_patches(&sur,&patpek);
      break;

      default:
      return(erpush("EX1412","SURFACE_AREA()"));
      }
/*
***Calculate surface area
*/
      c_case = 2;

      acc =  4; 

      status = varkon_sur_sarea
      ( &sur,patpek, &csys, acc, tol, c_case, 
        &sarea,  &cog,  &moi,  &axis1,  &axis2,  &axis3 );

   *p_area = sarea;

/*
***Free memory for patches
*/
    DBfree_patches(&sur,patpek);
/*
***Error handling     
*/
    if ( status < 0 ) return(erpush("EX2062","SURFACE_AREA"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    EXsear(
        DBId    *idpek,
        DBfloat  tol,
        DBfloat *p_area)

/*      Interface-routine for SECTION_AREA().
 *
 *      In: idpek  => Pekare till kurvans identitet.
 *          tol    => Tolerens.
 *          p_area => Pekare till utdata.
 *
 *      Ut: *p_area => Ber�knad area.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *      1997-12-20 sur302 tillagd  G. Liden
 *      1998-12-18 sur302->varkon_cur_secta  
 *                 sur715->varkon_cur_fromarc   
 *                 sur741->varkon_idpoint    G. Liden
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBCurve  cur;
    DBSeg *segpek;
    DBTmat csys;       /* Coordinate system for m.o.i.            */
    DBfloat  delta;      /* Calculation start accuracy value, equal */
                       /* to delta arclength for calculation      */
    DBint  c_case;     /* Calculation case:                       */
                       /* Eq.  1: Use only delta and not a_crit   */
                       /* Eq.  2: Use delta as start and a_crit   */
                       /* Eq. 11: As 1, but error unclosed curve  */
                       /* Eq. 12: As 2, but error unclosed curve  */
    DBfloat  maxdev;     /* Maximum deviation from plane            */
    DBfloat  sarea;      /* Surface area                            */
    DBVector  cog;     /* Center of gravity                       */
    DBVector  moi;     /* Moments of inertia (jx,jy,jz)           */
                       /* w.r.t to coordinate system p_csys       */
    DBVector  axis1;   /* Principal axis 1                        */
    DBVector  axis2;   /* Principal axis 2                        */
    DBVector  axis3;   /* Principal axis 3                        */
    bool   alloc1;
    DBArc  arc;        /* Arc section curve                       */
    DBSeg  arcseg1[4]; /* Arc segments for conversion to curve    */
    char   errbuf[80]; /* String for error message fctn erpush    */

/*
***Initiering.
*/
    alloc1  = FALSE;
    segpek  = NULL;
/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch ( typ )
      {
      case CURTYP:
      DBread_curve(&cur,NULL,&segpek,la);
      alloc1 = TRUE;
      break;

      case ARCTYP:
      DBread_arc(&arc,arcseg1, la);
      varkon_cur_fromarc(&arc,&cur,arcseg1);
      segpek = arcseg1;
      break;

      default:
      return(erpush("EX1412","SECTION_AREA()"));
      }
/*
***Ber�kna ytan.
*/
   delta  = F_UNDEF;
   c_case = 2;
   status = varkon_cur_secta(
                   &cur,segpek,&csys,delta,tol,c_case,&maxdev,
                   &sarea,&cog,&moi,&axis1,&axis2,&axis3);

   *p_area = sarea;
/*
***L�mna tillbaks allokerat minne.
*/
    if ( alloc1 ) DBfree_segments(segpek);
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(erpush("EX2062","SECTION_AREA"));
/*
***Varning om kurvan inte �r plan
*/
   if  (  maxdev > 10.0*varkon_idpoint() )
     {
     sprintf(errbuf,"%8.4f%%", maxdev );
     erinit();
     erpush("SU7411",errbuf);
     errmes();
     erinit();
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXsecg(
        DBId     *idpek,
        DBfloat   tol,
        DBVector *p_cgrav)

/*      Interface-routine for SECTION_CGRAV().
 *
 *      In: idpek  => Pekare till kurvans identitet.
 *          tol    => Tolerens.
 *          p_area => Pekare till utdata.
 *
 *      Ut: *p_area => Ber�knad area.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *      1997-12-20 sur302 tillagd  G. Liden
 *      1998-12-18 sur302->varkon_cur_secta  
 *                 sur715->varkon_cur_fromarc   
 *                 sur741->varkon_idpoint    G. Liden
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBCurve  cur;
    DBSeg *segpek;
    DBTmat csys;       /* Coordinate system for m.o.i.            */
    DBfloat  delta;      /* Calculation start accuracy value, equal */
                       /* to delta arclength for calculation      */
    DBint  c_case;     /* Calculation case:                       */
                       /* Eq.  1: Use only delta and not a_crit   */
                       /* Eq.  2: Use delta as start and a_crit   */
                       /* Eq. 11: As 1, but error unclosed curve  */
                       /* Eq. 12: As 2, but error unclosed curve  */
    DBfloat  maxdev;     /* Maximum deviation from plane            */
    DBfloat  sarea;      /* Surface area                            */
    DBVector  moi;     /* Moments of inertia (jx,jy,jz)           */
                       /* w.r.t to coordinate system p_csys       */
    DBVector  axis1;   /* Principal axis 1                        */
    DBVector  axis2;   /* Principal axis 2                        */
    DBVector  axis3;   /* Principal axis 3                        */
    bool   alloc1;
    DBArc  arc;        /* Arc section curve                       */
    DBSeg  arcseg1[4]; /* Arc segments for conversion to curve    */
    char   errbuf[80]; /* String for error message fctn erpush    */

/*
***Initiering.
*/
    alloc1  = FALSE;
    segpek  = NULL;
/*
***H�mta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och l�s geometridata.
*/
    switch ( typ )
      {
      case CURTYP:
      DBread_curve(&cur,NULL,&segpek,la);
      alloc1 = TRUE;
      break;

      case ARCTYP:
      DBread_arc(&arc,arcseg1, la);
      varkon_cur_fromarc(&arc,&cur,arcseg1);
      segpek = arcseg1;
      break;

      default:
      return(erpush("EX1412","SECTION_AREA()"));
      }
/*
***Ber�kna ytan.
*/
   delta  = F_UNDEF;
   c_case = 2;
   status = varkon_cur_secta(
                   &cur,segpek,&csys,delta,tol,c_case,&maxdev,
                   &sarea,p_cgrav,&moi,&axis1,&axis2,&axis3);
/*
***L�mna tillbaks allokerat minne.
*/
    if ( alloc1 ) DBfree_segments(segpek);
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(erpush("EX2062","SECTION_CGRAV"));
/*
***Varning om kurvan inte �r plan
*/
   if  (  maxdev > 10.0*varkon_idpoint() )
     {
     sprintf(errbuf,"%8.4f%%", maxdev );
     erinit();
     erpush("SU7411",errbuf);
     errmes();
     erinit();
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short    EXtxtl(
        char    *str,
        DBfloat *l)

/*      Interface-rutin f�r funktionen TEXTL. Ber�knar
 *      en textstr�ng:s l�ngd i mm.
 *
 *      In: str     => Textstr�ng.
 *
 *      Ut: *l => L�ngd.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 19/3/89 J. Kjellander
 *
 ******************************************************!*/

  {
   int n;

   n = strlen(str);

   *l = 0.01*defnap.tsize*defnap.twidth*(double)n + 
         0.0067*defnap.tsize*defnap.twidth*(double)(n-1);

   return(0);

  }

/********************************************************/
/*!******************************************************/

        short     EXsect(
        DBId     *idp1,
        DBId     *idp2,
        DBint     inr,
        DBint     alt,
        DBVector *vecptr)

/*      Interface-rutin f�r funktionen INTERSECT. Anropas
 *      med ID som indata. H�mtar geometridata f�r
 *      de refererade storheterna ur DB och anropar
 *      geo-rutinen GEintersect_pos() f�r att ber�kna sk�rningen.
 *
 *      In: idp1   => Pekare till 1:a storhetens identitet.
 *          idp2   => Pekare till 2:a storhetens identitet.
 *          inr    => Sk�rningsnummer.
 *          alt    => Typ av resultat.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *          EX1732 => Storheterna �r en och samma
 *          EX1972 => 2 plan
 *          EX1982 => Sk�rning saknas
 *          EX1992 => inr = 0
 *          EX2002 => inr < 0
 *
 *      (C)microform ab 11/1/85 J. Kjellander
 *
 *      3/1/86   Komposit-bug, J. Kjellander
 *      22/10/86 EX1732, J. Kjellander
 *      29/10/86 v3dbuf, J. Kjellander
 *      28/11/91 Sk�rning med plan, J. Kjellander
 *      20/12/91 inr = 0, J. Kjellander
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *      6/9/95   Yta-linje, J. Kjellander
 *      1997-01-27 inr > 1000, J.Kjellander
 *      1997-03-25 alt, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la1,la2;
    DBetype typ1,typ2;
    short   status;
    char    idstr1[V3STRLEN+1],idstr2[V3STRLEN+1];
    char   *pdat1=NULL,*pdat2=NULL;
    DBAny   gmstr1,gmstr2;
    DBSeg  *segpk1,*segpk2;
    DBSeg   arcsg1[4],arcsg2[4];
    DBPatch  *patpek;

/*
***Om alt != 0 anv�nder vi EXusec().
*/
    if ( alt != 0 ) return(EXusec(idp1,idp2,(short)inr,lsyspk,vecptr));
/*
***Kolla att sk�rningens ordningsnummer inte == 0.
*/
    if ( inr == 0 ) return(erpush("EX1992",""));
/*
***H�mta den 1:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp1, &la1, &typ1) < 0 )
      return ( erpush("EX1402",""));
/*
***H�mta den 2:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp2, &la2, &typ2) < 0 )
      return ( erpush("EX1402",""));
/*
***Kolla att det �r tv� olika storheter.
*/
    if ( la1 == la2 )
     {
     IGidst(idp1,idstr1);
     return(erpush("EX1732",idstr1));
     }
/*
***Kolla att inte koordinatsystem eller B-plan f�rekommer
***2 g�nger.
*/
    if ( (typ1 == CSYTYP  ||  typ1 == BPLTYP  ||  typ1 == SURTYP )  &&
         (typ2 == CSYTYP  ||  typ2 == BPLTYP  ||  typ2 == SURTYP ) )
      {
      IGidst(idp1,idstr1);
      IGidst(idp2,idstr2);
      strcat(idstr1,"%");
      strcat(idstr1,idstr2);
      return(erpush("EX1982",idstr1));
      }
/*
***L�s geometridata f�r storhet 1.
*/
    switch (typ1)
      {
      case LINTYP:
      DBread_line(&gmstr1.lin_un, la1);
      break;

      case ARCTYP:
      DBread_arc(&gmstr1.arc_un,arcsg1, la1);
      pdat1 = (char *)arcsg1;
      break;

      case CURTYP:
      DBread_curve(&gmstr1.cur_un,NULL,&segpk1,la1);
      pdat1 = (char *)segpk1;
      break;

      case CSYTYP:
      DBread_csys(&gmstr1.csy_un, NULL, la1);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr1.bpl_un, la1);
      break;

      case SURTYP:
      DBread_surface(&gmstr1.sur_un, la1);
      DBread_patches(&gmstr1.sur_un,&patpek);
      pdat1 = (char *)patpek;
      break;

      default:
      IGidst(idp1,idstr1);
      return(erpush("EX1412",idstr1));
      }
/*
***L�s geometridata f�r storhet 2.
*/
    switch (typ2)
      {
      case LINTYP:
      DBread_line(&gmstr2.lin_un, la2);
      break;

      case ARCTYP:
      DBread_arc(&gmstr2.arc_un,arcsg2,la2);
      pdat2 = (char *)arcsg2;
      break;

      case CURTYP:
      DBread_curve(&gmstr2.cur_un,NULL,&segpk2,la2);
      pdat2 = (char *)segpk2;
      break;

      case CSYTYP:
      DBread_csys(&gmstr2.csy_un, NULL, la2);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr2.bpl_un, la2);
      break;

      case SURTYP:
      DBread_surface(&gmstr2.sur_un, la2);
      DBread_patches(&gmstr2.sur_un,&patpek);
      pdat2 = (char *)patpek;
      break;

      default:
      IGidst(idp2,idstr2);
      if ( typ1 == CURTYP ) DBfree_segments(segpk1);
      if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
      return(erpush("EX1412",idstr2));
      }
/*
***Kolla att inte negativt inr anv�nds d�r det ej �r till�tet.
***Till�tet i alla l�gen d�r en linje eller 2D-cirkel figurerar
***annars inte.
*/
    if ( inr < 0 )
      {
      if ( typ1 == LINTYP  ||  typ2 == LINTYP ) ;
      else if ( typ1 == ARCTYP  &&  gmstr1.arc_un.ns_a == 0 ) ;
      else if ( typ2 == ARCTYP  &&  gmstr2.arc_un.ns_a == 0 ) ;
      else
        {
        IGidst(idp1,idstr1);
        IGidst(idp2,idstr2);
        strcat(idstr1,"%");
        strcat(idstr1,idstr2);
        erpush("EX1982",idstr1);
        status = erpush("EX2002","");
        goto end;
        }
      }
/*
***Ber�kna sk�rningen.
*/
    if ( (status=GEintersect_pos(&gmstr1,pdat1,&gmstr2,pdat2,
                                               lsyspk,inr,vecptr)) < 0 )
      {
      IGidst(idp1,idstr1);
      IGidst(idp2,idstr2);
      strcat(idstr1,"%");
      strcat(idstr1,idstr2);
      status = erpush("EX1982",idstr1);
      goto end;
      }
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);
/*
***Om n�gon av storheterna �r en kurva eller yta skall minne deallokeras
***innan vi avslutar.
*/
end:
    if ( typ1 == CURTYP ) DBfree_segments(segpk1);
    if ( typ2 == CURTYP ) DBfree_segments(segpk2);
    if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
    if ( typ2 == SURTYP ) DBfree_patches(&gmstr2.sur_un,patpek);

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short    EXnsec(
        DBId    *idp1,
        DBId    *idp2,
        DBshort  inr,
        DBTmat  *crdptr,
        DBshort *numint)

/*      Interface-rutin f�r funktionen N_INTERSECT.
 *
 *      In: idp1   => Pekare till 1:a storhetens identitet.
 *          idp2   => Pekare till 2:a storhetens identitet.
 *          inr    => -1 anger f�rl�ngning.
 *          crdptr => Pekare till lokalt koordinatsystem.
 *
 *      Ut: *numint => Antal sk�rningar.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *          EX1732 => Storheterna �r en och samma
 *          EX1972 => 2 plan
 *
 *      (C)microform ab 12/2/92 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *      6/9/95   Yta - linje, J. Kjellander
 *      1997-01-27 return(status), J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la1,la2;
    DBetype  typ1,typ2;
    short  status;
    char   idstr1[V3STRLEN+1],idstr2[V3STRLEN+1];
    char  *pdat1=NULL,*pdat2=NULL;
    DBAny  gmstr1,gmstr2;
    DBSeg  arcsg1[4],arcsg2[4];
    DBSeg *segpk1,*segpk2;
    DBPatch *patpek;

/*
***H�mta den 1:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp1, &la1, &typ1) < 0 )
      return ( erpush("EX1402",""));
/*
***H�mta den 2:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp2, &la2, &typ2) < 0 )
      return ( erpush("EX1402",""));
/*
***Kolla att det �r tv� olika storheter.
*/
    if ( la1 == la2 )
     {
     IGidst(idp1,idstr1);
     return(erpush("EX1732",idstr1));
     }
/*
***Kolla att inte koordinatsystem eller B-plan f�rekommer
***2 g�nger.
*/
    if ( (typ1 == CSYTYP  ||  typ1 == BPLTYP  ||  typ1 == SURTYP )  &&
         (typ2 == CSYTYP  ||  typ2 == BPLTYP  ||  typ2 == SURTYP ) )
      {
      IGidst(idp1,idstr1);
      IGidst(idp2,idstr2);
      strcat(idstr1,"%");
      strcat(idstr1,idstr2);
      return(erpush("EX1982",idstr1));
      }
/*
***Testa typ, l�s geometridata.
*/
    switch (typ1)
      {
      case LINTYP:
      DBread_line(&gmstr1.lin_un,la1);
      break;

      case ARCTYP:
      DBread_arc(&gmstr1.arc_un,arcsg1,la1);
      pdat1 = (char *)arcsg1;
      break;

      case CURTYP:
      DBread_curve(&gmstr1.cur_un,NULL,&segpk1,la1);
      pdat1 = (char *)segpk1;
      break;

      case CSYTYP:
      DBread_csys(&gmstr1.csy_un,NULL,la1);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr1.bpl_un,la1);
      break;

      case SURTYP:
      DBread_surface(&gmstr1.sur_un, la1);
      DBread_patches(&gmstr1.sur_un,&patpek);
      pdat1 = (char *)patpek;
      break;

      default:
      IGidst(idp1,idstr1);
      return(erpush("EX1412",idstr1));
      }

    switch (typ2)
      {
      case LINTYP:
      DBread_line(&gmstr2.lin_un,la2);
      break;

      case ARCTYP:
      DBread_arc(&gmstr2.arc_un,arcsg2,la2);
      pdat2 = (char *)arcsg2;
      break;

      case CURTYP:
      DBread_curve(&gmstr2.cur_un,NULL,&segpk2,la2);
      pdat2 = (char *)segpk2;
      break;

      case CSYTYP:
      DBread_csys(&gmstr2.csy_un,NULL,la2);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr2.bpl_un,la2);
      break;

      case SURTYP:
      DBread_surface(&gmstr2.sur_un, la2);
      DBread_patches(&gmstr2.sur_un,&patpek);
      pdat2 = (char *)patpek;
      break;

      default:
      IGidst(idp2,idstr2);
      if ( typ1 == CURTYP ) DBfree_segments(segpk1);
      if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
      return(erpush("EX1412",idstr2));
      }
/*
***Kolla att inte negativt inr anv�nds d�r det ej �r till�tet.
***Till�tet i alla l�gen d�r en linje eller 2D-cirkel figurerar
***annars inte.
*/
    if ( inr < 0 )
      {
      if ( typ1 == LINTYP  ||  typ2 == LINTYP ) ;
      else if ( typ1 == ARCTYP  &&  gmstr1.arc_un.ns_a == 0 ) ;
      else if ( typ2 == ARCTYP  &&  gmstr2.arc_un.ns_a == 0 ) ;
      else
        {
        IGidst(idp1,idstr1);
        IGidst(idp2,idstr2);
        strcat(idstr1,"%");
        strcat(idstr1,idstr2);
        erpush("EX1982",idstr1);
        status = erpush("EX2002","");
        goto end;
        }
      }
/*
***Ber�kna antal sk�rningar.
*/
    status = GEintersect_npos(&gmstr1,pdat1,&gmstr2,pdat2,crdptr,inr,numint);
/*
***Om n�gon av storheterna �r en kurva eller yta skall minne deallokeras.
*/
end:
    if ( typ1 == CURTYP ) DBfree_segments(segpk1);
    if ( typ2 == CURTYP ) DBfree_segments(segpk2);
    if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
    if ( typ2 == SURTYP ) DBfree_patches(&gmstr2.sur_un,patpek);

    return(status);
  }

/********************************************************/
/*!******************************************************/

       short    EXidnt(
       DBetype *typmsk,
       DBId     ident[],
       bool    *end,
       bool    *right)

/*      Funktionen IDENT.
 *
 *      In: typmsk => Pekare till typmask.
 *          ident  => Array av DBId.
 *          end    => Pekare till �nde
 *          right  => Pekare till sida
 *
 *      Ut: *ident => Identitet f�r utpekad storhet.
 *          *end   => �nde
 *          *right => Sida
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 22/11/85 J. Kjellander
 *
 *      20/11/92 end och right, J. Kjellander
 *
 ******************************************************!*/

  {

    if ( IGgsid(ident,typmsk,end,right,(short)0) < 0 )
      {
      ident->seq_val = 0;
      ident->ord_val = 1;
      ident->p_nextre = NULL;
      }

    WPerhg();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short   EXpos(
       double *px,
       double *py,
       char   *pc)

/*      Funktionen POS.
 *
 *      In: px => Pekare till X-koordinat.
 *          py => Pekare till Y-koordinat.
 *          pc => Pekare till pektecknet.
 *
 *      Ut: *px => Modellkoordinat-X
 *          *py => Modellkoordinat-Y
 *          *pc => Pektecknet.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 22/11/85 J. Kjellander
 *
 *       18/5/92 Pektecken, J. Kjellander
 *
 ******************************************************!*/

  {
    DBVector pos;
    wpw_id   grw_id;

    WPgmc2(TRUE,pc,px,py,&grw_id);

    if ( lsyspk != NULL )
      {
      pos.x_gm = *px;
      pos.y_gm = *py;
      pos.z_gm = 0.0;
      GEtfpos_to_local(&pos,lsyspk,&pos);
     *px = pos.x_gm;
     *py = pos.y_gm;
      }

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short    EXscr(
       DBshort *pix,
       DBshort *piy,
       DBint   *win_id)

/*      SCREEN() function in MBS.
 *
 *      In: pix    => Pekare till utdata.
 *          piy    => Pekare till utdata.
 *          win_id => Pekare till utdata.
 *
 *      Ut: *pix    => Sk�rmkoordinat-X
 *          *piy    => Sk�rmkoordinat-Y
 *          *win_id => F�nsterid.
 *
 *      FV:      0 = Ok.
 *          REJECT = Avbruten operation.
 *
 *      (C)microform ab 22/11/85 J. Kjellander
 *
 *      22/1-95  Multif�nster, J. Kjellander
 *
 ******************************************************!*/

  {
   char c;

   WPgtsc(FALSE,&c,pix,piy,win_id);

   return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short     EXarea(
       DBId     *ridvek,
       DBshort   nref,
       DBfloat   dist,
       DBfloat  *area,
       DBVector *tp)

/*      Calculate area and centre of gravity.
 *      Used by MBS-functions AREA() and CGRAV().
 *
 *      In: ridvek => C ptr to array of refs.
 *          nref   => Number of refs.
 *          dist   => Stepsize.
 *
 *      Out: *area  => Area.
 *           *tp    => Centre of gravity.
 *
 *      Return:  0 = Ok.
 *          EX1402 = Entity does not exist in DB
 *          EX1412 = Illegal entity type
 *          EX1962 = malloc() error.
 *
 *      (C)microform ab 26/7/90 J. Kjellander
 *
 *      7/6/93     Dynamiska segment, J. Kjellander
 *      2007-09-30 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    DBptr    la;
    DBetype  typ;
    short    nlin,narc,ncur,status;
    DBLine  *lpvek[GMMXXH],lin[GMMXXH];
    DBArc   *apvek[GMMXXH],arc[GMMXXH];
    DBSeg   *aspvek[GMMXXH],arcseg[GMMXXH][4];
    DBCurve *cpvek[GMMXXH],cur[GMMXXH];
    DBSeg   *cspvek[GMMXXH];
    DBVector pos;

/*
***Get the geometry of all referenced entities and save
***in mallocated C memory.
*/
    nlin = narc = ncur = 0;

    for ( i=0; i<nref ; ++i )
      {
      if ( DBget_pointer('I',&ridvek[i],&la,&typ) < 0 ) 
        return(erpush("EX1402",""));

      switch ( typ )
         {
         case (LINTYP):
         DBread_line(&lin[nlin],la);
         lpvek[nlin] = &lin[nlin];
         if ( lsyspk != NULL ) GEtfLine_to_local(lpvek[nlin],lsyspk,lpvek[nlin]);
       ++nlin;
         break;

         case (ARCTYP):
         DBread_arc(&arc[narc],&arcseg[narc][0],la);
         if ( arc[narc].ns_a == 0 )
           {
           pos.x_gm = arc[narc].x_a;
           pos.y_gm = arc[narc].y_a;
           pos.z_gm = 0.0;

           if ( GE300(&pos,arc[narc].r_a,arc[narc].v1_a,arc[narc].v2_a,NULL,
                 &arc[narc],&arcseg[narc][0],3) < 0 ) return(erpush("GE7213","GE300"));
           }
         apvek[narc] = &arc[narc];
         aspvek[narc] = &arcseg[narc][0];
         if ( lsyspk != NULL )
           GEtfArc_to_local(apvek[narc],aspvek[narc],lsyspk,apvek[narc],aspvek[narc]);
       ++narc;
         break;

         case (CURTYP):
         DBread_curve(&cur[ncur],NULL,&cspvek[ncur],la);
         cpvek[ncur] = &cur[ncur];
         if ( lsyspk != NULL )
           GEtfCurve_to_local(cpvek[narc],cspvek[narc],NULL,lsyspk,cpvek[narc],cspvek[narc],NULL);
       ++ncur;
         break;

         default:
         return(erpush("EX1412",""));
         }
      }
/*
***Calculate area and centre of gravity.
*/
    if ( GEarea2D(lpvek,nlin,apvek,aspvek,narc,cpvek,cspvek,ncur,dist,area,tp) < 0 )
      status = erpush("EX1552","");
    else
      status = 0;
/*
***Deallocate C memory.
*/
    if ( ncur > 0 )
      {
      for ( i=0; i<ncur; ++i ) DBfree_segments(cspvek[i]);
      }
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short     EXusec(
        DBId     *idp1,
        DBId     *idp2,
        DBshort   inr,
        DBTmat   *crdptr,
        DBVector *vecptr)

/*      Anv�nds av EXsect() om man beg�rt UV-v�rde som utdata.
 *
 *      In: idp1   => Pekare till 1:a storhetens identitet.
 *          idp2   => Pekare till 2:a storhetens identitet.
 *          inr    => Sk�rningsnummer.
 *          crdptr => Pekare till lokalt koordinatsystem.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otill�ten typ 
 *          EX1732 => Storheterna �r en och samma
 *          EX1972 => 2 plan
 *          EX1982 => Sk�rning saknas
 *          EX1992 => inr = 0
 *          EX2002 => inr < 0
 *
 *      1996-08-19 Modified EXe1:EXsect(), Gunnar Liden
 *
 *      1997-02-27 Error message typ     
 *                 Line/surface intersect Gunnar Liden
 *      1997-05-16 Elimination of compiler warnings
 *      1998-04-06 inr no longer > 1000
 *      1998-04-08 Flyttad till EXe1.c, J.Kjellander
 *      1999-12-18 sur164->varkon_sur_curint G Liden
 *
 ******************************************************!*/

  {
    DBptr    la1,la2;
    DBetype  typ1,typ2;
    short    status;
    char     idstr1[V3STRLEN+1],idstr2[V3STRLEN+1];
    char    *pdat1,*pdat2;
    DBAny    gmstr1,gmstr2;
    DBSeg   *segpk1,*segpk2;
    DBSeg    arcsg1[4],arcsg2[4];
    DBPatch *patpek;
    char     errbuf[80];  /* String for error message  */

/*
***Parameters for varkon_sur_curint
*/
   DBint  ocase,acc,sol,nsol;

   DBVector  start,r3_pt,uv_pt,
          all_uv[25],all_xyz[25];
   DBfloat  all_u[25];    /* All curve parameter values */
   DBfloat  uvalue; 
/*
***Initialization of internal variables 
*/
  pdat1 = NULL;
  pdat2 = NULL;
/*
***Initializations for surface/curve intersect
***No start point.
*/
   start.x_gm =  0.0;
   start.y_gm =  0.0;
   start.z_gm = -1.0;
/*
***Sortering (ocase) relativt kurvans startpunkt.
***Ber�kningsfall (acc) dvs. alla patchar. Ingen startpunkt given.
***�nskad l�sning = 1 
***Antal l�sningar innan anropet = 0.
*/
   ocase = 1 + 1000;
   acc   = 1;
   sol   = inr;
   nsol  = 0;
/*
***Kolla att sk{rningens ordningsnummer inte == 0.
*/
    if ( inr == 0 ) return(erpush("EX1992",""));
/*
***H�mta den 1:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp1, &la1, &typ1) < 0 )
      return ( erpush("EX1402",""));
/*
***H�mta den 2:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp2, &la2, &typ2) < 0 )
      return ( erpush("EX1402",""));
/*
***Kolla att det �r tv� olika storheter.
*/
    if ( la1 == la2 )
     {
     IGidst(idp1,idstr1);
     return(erpush("EX1732",idstr1));
     }
/*
***Kolla att inte koordinatsystem eller B-plan f�rekommer
***2 g�nger.
*/
    if ( (typ1 == CSYTYP  ||  typ1 == BPLTYP  ||  typ1 == SURTYP )  &&
         (typ2 == CSYTYP  ||  typ2 == BPLTYP  ||  typ2 == SURTYP ) )
      {
      IGidst(idp1,idstr1);
      IGidst(idp2,idstr2);
      strcat(idstr1,"%");
      strcat(idstr1,idstr2);
      return(erpush("EX1982",idstr1));
      }
/*
***L�s geometridata f�r storhet 1.
*/
    switch (typ1)
      {
      case LINTYP:
      DBread_line(&gmstr1.lin_un, la1);
      break;

      case ARCTYP:
      DBread_arc(&gmstr1.arc_un,arcsg1, la1);
      pdat1 = (char *)arcsg1;
      break;

      case CURTYP:
      DBread_curve(&gmstr1.cur_un,NULL,&segpk1,la1);
      pdat1 = (char *)segpk1;
      break;

      case CSYTYP:
      DBread_csys(&gmstr1.csy_un, NULL, la1);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr1.bpl_un, la1);
      break;

      case SURTYP:
      DBread_surface(&gmstr1.sur_un, la1);
      DBread_patches(&gmstr1.sur_un,&patpek);
      pdat1 = (char *)patpek;
      break;

      default:
      IGidst(idp1,idstr1);
      return(erpush("EX1412",idstr1));
      }
/*
***L�s geometridata f�r storhet 2.
*/
    switch (typ2)
      {
      case LINTYP:
      DBread_line(&gmstr2.lin_un, la2);
      break;

      case ARCTYP:
      DBread_arc(&gmstr2.arc_un,arcsg2,la2);
      pdat2 = (char *)arcsg2;
      break;

      case CURTYP:
      DBread_curve(&gmstr2.cur_un,NULL,&segpk2,la2);
      pdat2 = (char *)segpk2;
      break;

      case CSYTYP:
      DBread_csys(&gmstr2.csy_un, NULL, la2);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr2.bpl_un, la2);
      break;

      case SURTYP:
      DBread_surface(&gmstr2.sur_un, la2);
      DBread_patches(&gmstr2.sur_un,&patpek);
      pdat2 = (char *)patpek;
      break;

      default:
      IGidst(idp2,idstr2);
      if ( typ1 == CURTYP ) DBfree_segments(segpk1);
      if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
      return(erpush("EX1412",idstr2));
      }
/*
***Kolla att inte negativt inr anv�nds d�r det ej �r till�tet.
***Till�tet i alla l�gen d�r en linje eller 2D-cirkel figurerar
***annars inte.
*/
    if ( inr < 0 )
      {
      if ( typ1 == LINTYP  ||  typ2 == LINTYP ) ;
      else if ( typ1 == ARCTYP  &&  gmstr1.arc_un.ns_a == 0 ) ;
      else if ( typ2 == ARCTYP  &&  gmstr2.arc_un.ns_a == 0 ) ;
      else
        {
        IGidst(idp1,idstr1);
        IGidst(idp2,idstr2);
        strcat(idstr1,"%");
        strcat(idstr1,idstr2);
        erpush("EX1982",idstr1);
        status = erpush("EX2002","");
        goto end;
        }
      }
/*
***Ber�kna sk�rningen.
***Bara f�r yta/kurva.
*/
   if     ( typ1 == CURTYP  &&  typ2 == SURTYP )
     {
/*
***Ber�kna sk�rningar. Sk�rningarna returneras i all_uv som h�r
***deklarerats till 25 element men som borde ha samma deklaration 
***som motsvarande variabel i surpac n�mligen SMAX fn. = 10.
*/
     status = varkon_sur_curint((DBSurf*)&gmstr2,(DBPatch *)pdat2,
             (DBCurve*)&gmstr1,(DBSeg *)pdat1,&start,ocase,acc,sol,
             &nsol,&r3_pt,&uv_pt,&uvalue,all_u,all_uv,all_xyz);
     }
   else if ( typ1 == SURTYP  &&  typ2 == CURTYP )
     {
     status = varkon_sur_curint((DBSurf*)&gmstr1,(DBPatch *)pdat1,
              (DBCurve*)&gmstr2,(DBSeg *)pdat2,&start,ocase,acc,sol,
               &nsol,&r3_pt,&uv_pt,&uvalue,all_u,all_uv,all_xyz);
     }
/*
***Linje - yta. Oavsett i vilken ordning de kommer l�gger
***geo723() alltid linjens parameterv�rden i uout1 s� att
***GEposition() kan ber�kna position.
*/
   else if ( typ1 == LINTYP  &&  typ2 == SURTYP )
     {
     uv_pt.z_gm = F_UNDEF;
     status = GEintersect_pv(&gmstr2,pdat2,
                     &gmstr1,NULL,NULL,sol,&uv_pt.x_gm,&uv_pt.y_gm);
       if ( status < 0 ) return(erpush("GE7112",""));
     }
   else if ( typ2 == LINTYP  &&  typ1 == SURTYP )
     {
     uv_pt.z_gm = F_UNDEF;
     status = GEintersect_pv(&gmstr1,pdat1,
                     &gmstr2,NULL,NULL,sol,&uv_pt.x_gm,&uv_pt.y_gm);
       if ( status < 0 ) return(erpush("GE7112",""));
     }
   else
     {
     erinit();
     sprintf(errbuf,"types %d %d not OK%%EXe1_gl",typ1,typ2);
     status= erpush("SU2993",errbuf);
     return(status);
     }
/*
***Felhantering.
*/
  if ( status < 0 )
    {
    IGidst(idp1,idstr1);
    IGidst(idp2,idstr2);
    strcat(idstr1,"%");
    strcat(idstr1,idstr2);
    status = erpush("EX1982",idstr1);
    goto end;
    }
/*
***Utdata
*/
   vecptr->x_gm  = uv_pt.x_gm - 1.0;
   vecptr->y_gm  = uv_pt.y_gm - 1.0;
   vecptr->z_gm  = uvalue     - 1.0;       
/*
***Om n}gon av storheterna {r en kurva eller yta skall minne deallokeras
***innan vi avslutar.
*/
end:
    if ( typ1 == CURTYP ) DBfree_segments(segpk1);
    if ( typ2 == CURTYP ) DBfree_segments(segpk2);
    if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
    if ( typ2 == SURTYP ) DBfree_patches(&gmstr2.sur_un,patpek);

    return(status);
  }

/********************************************************/
/*!******************************************************/

       short     EXpinc(
       DBVector *p1,
       DBVector *p2,
       DBfloat   r1,
       DBfloat   r2,
       DBint     npos,
       DBVector *pos,
       DBint    *pst,
       bool     *any)

/*      Executes MBS function POS_IN_CONE().
 *
 *
 *      In: p1     => Ptr to cone start
 *          p2     => Ptr to cone end
 *          r1     => Start radius.
 *          r2     => End radius
 *          npos   => Number of positions
 *          pos    => Ptr to position coordinates
 *          pst    => Ptr to result or NULL
 *          any    => Ptr to result
 *
 *      Out: *pst  => Status for all positions
 *           *any  => TRUE if one or more inside
 *
 *      Return: 0  => Ok.
 *
 *      (C)2005-08-04 J. Kjellander, �rebro university
 *
 ******************************************************!*/

  {
   return(GEpos_in_cone(p1,p2,r1,r2,npos,pos,pst,any));
  }
  
/********************************************************/
