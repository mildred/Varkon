/*!*****************************************************
*
*    exref.c 
*    =======
*
*    EXrstr();      Interface routine for RSTR
*    EXrval();      Interface routine for RVAL
*    EXglor();      Interface routine for GLOBAL_REF
*    EXfins();      Interface routine for FINDS
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"
#include <string.h>

/*!******************************************************/

        short EXrstr(
        DBId    *id,
        DBshort  form,
        char    *idstr)
 
/*      Interface-rutin f�r RSTR. Konverterar en 
 *      referens till str�ng.
 *
 *      In:  id    => Pekare till referens.
 *           form  => F�ltvidd.
 *           idstr => Pekare till resultat.
 *
 *      Ut: *idstr => Str�ng.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 7/10/86 R. Svedin
 *
 ******************************************************!*/

  {
    IGidst(id,idstr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXrval(
        char *idstr,
        DBId *id)

/*      Interface-rutin f�r RVAL. Konverterar en str�ng
 *      till en referens.
 *
 *      In: idstr  => Pekare till en str�ng.
 *          id     => Pekare till Resultat.
 *
 *      Ut: *id    => Referens.
 *
 *      Felkoder: EX1782 = Syntaxfel
 *
 *      (C)microform ab 7/10/86 R. Svedin
 *
 *      22/12/86 Bug felkod, J. Kjellander
 *
 ******************************************************!*/

  {
    if ( IGstid(idstr,id) < 0 ) return(erpush("EX1782",idstr));
    else return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXglor(
        DBId    *inref,
        DBshort  lev,
        DBId    *utref)

/*      Interface-rutin f�r GLOBAL_REF.
 *
 *      In: inref = Pekare till lokal referens.
 *          lev   = Antal niv�er bak�t (upp�t).
 *          utref = Pekare till resultat.
 *
 *      Ut: *utref = Global referens.
 *
 *      Felkoder: EX1772 = Lev har otill�tet v�rde
 *
 *      (C)microform ab 21/12/86 J. Kjellander
 *
 ******************************************************!*/

  {

    if ( lev < 0 || lev > MXINIV ) return(erpush("EX1772",""));

    return(gmglor(inref,lev,utref));
  }

/********************************************************/
/*!******************************************************/

        short EXfins(
        char     str1[],
        char     str2[],
        DBshort *strpos)

/*      Interface-rutin f�r FINDS. S�ker en str�ng i
 *      en annan str�ng.
 *
 *      In: str1   = Pekare till str�ng att s�ka i.
 *          str2   = Pekare till s�kt str�ng.
 *
 *      Ut:  *strpos = 1:a pos. i str1 d�r str2 f�rekommer.
 *
 *      Felkoder: Inga.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      20/4/87  Kodning, J. Kjellander
 *
 ******************************************************!*/

  {
    int i,l1,l2;

/*
***Ber�kna str�ngarnas l�ngd.
*/
    l2 = strlen(str2);
    l1 = strlen(str1);
/*
***Om str2 �r l�ngre �n str1 kan resultatet bara bli noll.
*/
    *strpos = 0;
    if ( l2 > l1 ) return(0);
/*
***Justera l1 s� att s�kning i str1 bara p�g�r s� l�nge
***resten av l1 �r tillr�kligt l�ng f�r l2.
*/
    l1 = l1 - l2 + 1;

    for ( i=0 ; i<l1 ; ++i )
      {
      if ( str1[i] == *str2 )
        {
        if ( strncmp(&str1[i],str2,l2) == 0 )
          {
          *strpos = i+1;
          return(0);
          }
        }
      }

    return(0);
  }

/********************************************************/
