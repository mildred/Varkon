/**********************************************************************
*
*    dbdim.c
*    =======
*
*
*    This file includes the following public functions:
*
*    DBinsert_ldim();   Inserts a ldim entity
*    DBread_ldim();     Reads a ldim entity
*    DBupdate_ldim();   Updates a ldim entity
*    DBdelete_ldim();   Deletes a ldim entity
*
*    DBinsert_cdim();   Inserts a cdim entity
*    DBread_cdim();     Reads a cdim entity
*    DBupdate_cdim();   Updates a cdim entity
*    DBdelete_cdim();   Deletes a cdim entity
*
*    DBinsert_rdim();   Inserts a rdim entity
*    DBread_rdim();     Reads a rdim entity
*    DBupdate_rdim();   Updates a rdim entity
*    DBdelete_rdim();   Deletes a rdim entity
*
*    DBinsert_adim();   Inserts a adim entity
*    DBread_adim();     Reads a adim entity
*    DBupdate_adim();   Updates a adim entity
*    DBdelete_adim();   Deletes a adim entity
*
*    This file is part of the VARKON Database Library.
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_ldim(
        GMLDM   *ldmpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin f�r lagring av l�ngdm�tt. Post-specifika
 *      data fylls i och posten lagras d�refter med ett
 *      anrop till inpost().
 *
 *      In: ldmpek => Pekare till en ldm-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till ldm-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanf�r virtuellt omr�de.
 *          -2  => IDTAB full.
 *          -3  => Data f�r inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    ldmpek->hed_ld.type = LDMTYP;   /* Typ = l�ngdm�tt */
    ldmpek->hed_ld.vers = GMPOSTV1; /* Version */
/*
***Lagra.
*/
    return(inpost((GMUNON *)ldmpek,idpek,lapek,sizeof(GMLDM)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_ldim(
        GMLDM   *ldmpek,
        DBptr    la)

/*      L�sning av l�ngdm�tt-post.
 *
 *      In: ldmpek => Pekare till en ldm-structure.
 *          la     => M�ttets adress i GM.
 *
 *      Ut: *ldmpek => L�ngdm�tt-post.
 *
 *      FV:   0 => Ok.
 *          < 0 => Fel fr�n rddat1().
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME(hedpek,ldmpek,sizeof(GMLDM));
      break;
 
      default:
      V3MOME(hedpek,ldmpek,sizeof(GMLDM0));
      ldmpek->pcsy_ld = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_ldim(
        GMLDM   *ldmpek,
        DBptr    la)

/*      Skriver �ver en existerande l�ngdm�tt-post.
 *
 *      In: ldmpek => Pekare till en ldm-structure.
 *          la     => M�ttets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      updata( (char *)ldmpek, la, sizeof(GMLDM));
      break;
 
      default:
      updata( (char *)ldmpek, la, sizeof(GMLDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_ldim(DBptr la)

/*      Stryker ett l�ngdm�tt och deallokerar allokerat minne.
 *
 *      In: la => M�ttets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensr�knare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      rldat1(la,sizeof(GMLDM));
      break;
 
      default:
      rldat1(la,sizeof(GMLDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBinsert_cdim(
        GMCDM   *cdmpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin f�r lagring av diameterm�tt. Post-specifika
 *      data fylls i och posten lagras d�refter med ett
 *      anrop till inpost().
 *
 *      In: cdmpek => Pekare till en cdm-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till cdm-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanf�r virtuellt omr�de.
 *          -2  => IDTAB full.
 *          -3  => Data f�r inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    cdmpek->hed_cd.type = CDMTYP;   /* Typ = diameterm�tt */
    cdmpek->hed_cd.vers = GMPOSTV1; /* Version */
/*
***Lagra.
*/
    return(inpost((GMUNON *)cdmpek,idpek,lapek,sizeof(GMCDM)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_cdim(
        GMCDM   *cdmpek,
        DBptr    la)

/*      L�sning av diameterm�tt-post.
 *
 *      In: cdmpek => Pekare till en cdm-structure.
 *          la     => M�ttets adress i GM.
 *
 *      Ut: *cdmpek => Diameterm�tt-post.
 *
 *      FV:   0 => Ok.
 *          < 0 => Fel fr�n rddat1().
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME(hedpek,cdmpek,sizeof(GMCDM));
      break;
 
      default:
      V3MOME(hedpek,cdmpek,sizeof(GMCDM0));
      cdmpek->pcsy_cd = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_cdim(
        GMCDM   *cdmpek,
        DBptr    la)

/*      Skriver �ver en existerande diameterm�tt-post.
 *
 *      In: cdmpek => Pekare till en cdm-structure.
 *          la     => M�ttets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      updata( (char *)cdmpek, la, sizeof(GMCDM));
      break;
 
      default:
      updata( (char *)cdmpek, la, sizeof(GMCDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_cdim(DBptr la)

/*      Stryker ett diameter-m�tt och deallokerar
 *      allokerat minne.
 *
 *      In: la => M�ttets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensr�knare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      rldat1(la,sizeof(GMCDM));
      break;
 
      default:
      rldat1(la,sizeof(GMCDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBinsert_rdim(
        GMRDM   *rdmpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin f�r lagring av radiem�tt. Post-specifika
 *      data fylls i och posten lagras d�refter med ett
 *      anrop till inpost().
 *
 *      In: rdmpek => Pekare till en rdm-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till rdm-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanf�r virtuellt omr�de.
 *          -2  => IDTAB full.
 *          -3  => Data f�r inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    rdmpek->hed_rd.type = RDMTYP;   /* Typ = radiem�tt */
    rdmpek->hed_rd.vers = GMPOSTV1; /* Version */
/*
***Lagra.
*/
    return(inpost((GMUNON *)rdmpek,idpek,lapek,sizeof(GMRDM)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_rdim(
        GMRDM   *rdmpek,
        DBptr    la)

/*      L�sning av radiem�tt-post.
 *
 *      In: rdmpek => Pekare till en rdm-structure.
 *          la     => M�ttets adress i GM.
 *
 *      Ut: *rdmpek => Radiem�tt-post.
 *
 *      FV:   0 => Ok.
 *          < 0 => Fel fr�n rddat1().
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME(hedpek,rdmpek,sizeof(GMRDM));
      break;
 
      default:
      V3MOME(hedpek,rdmpek,sizeof(GMRDM0));
      rdmpek->pcsy_rd = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_rdim(
        GMRDM   *rdmpek,
        DBptr    la)

/*      Skriver �ver en existerande radiem�tt-post.
 *
 *      In: rdmpek => Pekare till en rdm-structure.
 *          la     => M�ttets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      updata( (char *)rdmpek, la, sizeof(GMRDM));
      break;
 
      default:
      updata( (char *)rdmpek, la, sizeof(GMRDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_rdim(DBptr la)

/*      Stryker ett radie-m�tt och deallokerar
 *      allokerat minne.
 *
 *      In: la => M�ttets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensr�knare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      rldat1(la,sizeof(GMRDM));
      break;
 
      default:
      rldat1(la,sizeof(GMRDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBinsert_adim(
        GMADM   *admpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin f�r lagring av vinkelm�tt. Post-specifika
 *      data fylls i och posten lagras d�refter med ett
 *      anrop till inpost().
 *
 *      In: admpek => Pekare till en adm-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till adm-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanf�r virtuellt omr�de.
 *          -2  => IDTAB full.
 *          -3  => Data f�r inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    admpek->hed_ad.type = ADMTYP;   /* Typ = vinkelm�tt */
    admpek->hed_ad.vers = GMPOSTV1; /* Version */
/*
***Lagra.
*/
    return(inpost((GMUNON *)admpek,idpek,lapek,sizeof(GMADM)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_adim(
        GMADM   *admpek,
        DBptr    la)

/*      L�sning av vinkelm�tt-post.
 *
 *      In: admpek => Pekare till en adm-structure.
 *          la     => M�ttets adress i GM.
 *
 *      Ut: *admpek => Vinkelm�tt-post.
 *
 *      FV:   0 => Ok.
 *          < 0 => Fel fr�n rddat1().
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME(hedpek,admpek,sizeof(GMADM));
      break;
 
      default:
      V3MOME(hedpek,admpek,sizeof(GMADM0));
      admpek->pcsy_ad = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_adim(
        GMADM   *admpek,
        DBptr    la)

/*      Skriver �ver en existerande vinkelm�tt-post.
 *
 *      In: admpek => Pekare till en adm-structure.
 *          la     => M�ttets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      updata( (char *)admpek, la, sizeof(GMADM));
      break;
 
      default:
      updata( (char *)admpek, la, sizeof(GMADM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_adim(DBptr la)

/*      Stryker ett vinkel-m�tt och deallokerar
 *      allokerat minne.
 *
 *      In: la => M�ttets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensr�knare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      rldat1(la,sizeof(GMADM));
      break;
 
      default:
      rldat1(la,sizeof(GMADM0));
      break;
      }

    return(0);
  }

/********************************************************/
