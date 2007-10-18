/**********************************************************************
*
*    db4.c
*    =====
*
*
*    This file includes the following internal functions:
*
*    gmunlk();   Unlinks one entity
*    gmglor();   Creates global ref
*    gmmtm();    Makes entity temporary
*    gmumtm();   Unmakes entity temporary
*    gmrltm();   Deletes temporary entity
*
*
*    This file is part of the VARKON Database Library.
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
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus gmunlk(DBptr la)

/*      L�nkar bort en storhet. Om storheten �r en
 *      av flera instanser l�nkas den bort ur instans-
 *      listan. Om det �r sista storheten i listan
 *      stryks �ven ID ur storhetens ID-tabell. Om
 *      storheten ing�r i en grupp tas den bort ur
 *      gruppen/grupperna. Rutinen f�ruts�tter att
 *      en storhet verkligen finns vid la.
 *
 *      In: la => Storhetens la.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 1/2/86 J. Kjellander
 *
 ******************************************************!*/

  {
    GMPRT  part;
    GMRECH hed,hed_tmp;
    DBint i;
    DBptr la_first,la_tmp,la_next;
    DBptr suddad = ERASED;

/*
***L�s storhetens header.
*/
    DBread_header(&hed,la);
/*
***Ta bort eventuella medlemsskap i grupp.
*/
    for ( i=0; i<3; ++i )
       if ( hed.g_ptr[i] != DBNULL )
         {
         DBdelete_group_member(hed.g_ptr[i],la);
         hed.g_ptr[i] = DBNULL;
         }
/*
***L�s la till f�rsta instans ur den part-post som
***utg�r �gare till storheten.
*/
    DBread_part(&part,hed.p_ptr);
    la_tmp = part.itp_pt+hed.seknr*sizeof(DBptr);
    rddat1((char *)&la_first,la_tmp,sizeof(DBptr));
/*
***Om storheten �r 1:a instansen, skriv in storhetens
***next-pekare i ID-tabellen.
*/
    if ( la_first == la)
      {
      if ( hed.n_ptr == DBNULL )
        {
        updata((char *)&suddad,la_tmp,sizeof(DBptr));
        }
      else
        {
        la_next = hed.n_ptr;
        updata((char *)&la_next,la_tmp,sizeof(DBptr));
        }
      }
/*
***Storheten var inte 1:a instansen. Ta reda p� la
***till n�rmast f�reg�ende instans.
*/
    else
      {
      la_tmp = la_first;
loop:
      DBread_header(&hed_tmp,la_tmp);
      if ( hed_tmp.n_ptr != la )
        {
        la_tmp = hed_tmp.n_ptr;
        goto loop;
        }
/*
***Skriv in storhetens next-pekare i f�reg�ende
***storhets huvud.
*/
      hed_tmp.n_ptr = hed.n_ptr;
      DBupdate_header(&hed_tmp,la_tmp);
      }
/*
***Nollst�ll storhetens next-pekare och skriv
***tillbaks huvudet.
*/
    hed.n_ptr = DBNULL;
    DBupdate_header(&hed,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmglor(DBId *inref, DBshort lev, DBId *utref)

/*      Genererar global referens.
 *
 *      In: inref = Pekare till lokal referens
 *          lev   = Antal niv�er bak�t 0 <= lev <= MXINIV
 *          utref = Pekare till resultat
 *
 *      Ut: *utref = Global referens.
 *
 *      Felkod: GM0082 = F�r m�nga niv�er i utref
 *
 *      (C)microform ab 22/12/86 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   prtpek;
    DBshort i,j,utlev;
    GMRECH  hed;
    DBId    tmpref[MXINIV];

/*
***Backa lev niv�er om m�jligt.
*/
    prtpek = actprt;

    while ( lev > 0  &&  prtpek != huvprt )
      {
      DBread_header(&hed,prtpek);
      prtpek = hed.p_ptr;
      --lev;
      }
/*
***Utg�ende fr�n denna niv�, generera dess globala ID
***bakl�nges i tmpref.
*/
    utlev = 0;

    while ( prtpek != huvprt )
      {
      DBread_header(&hed,prtpek);
      tmpref[utlev].seq_val = hed.seknr;
      tmpref[utlev].ord_val = hed.ordnr;
      prtpek = hed.p_ptr;
      ++utlev;
      }
/*
***Kopiera tmpref till utref fast i omv�nd ordning.
*/
    for ( i=0; i<utlev; ++i )
      {
      (utref+i)->seq_val = tmpref[utlev-i-1].seq_val;
      (utref+i)->ord_val = tmpref[utlev-i-1].ord_val;
      (utref+i)->p_nextre = &utref[i+1];
      }
/*
***L�gg till inref sist i utref. Om inref �r en global
***referens konverteras den nu till lokal.
*/
    for ( j=0; ; ++j, ++i )
      {
      if ( i > MXINIV-1 ) return(erpush("DB0082",""));
      (utref+i)->seq_val = abs((inref+j)->seq_val);
      (utref+i)->ord_val = (inref+j)->ord_val;
      (utref+i)->p_nextre = &utref[i+1];
      if ( (inref+j)->p_nextre == NULL ) 
        {
        (utref+i)->p_nextre = NULL;
        goto end;
        }
      } 
/*
***G�r utref till global referens.
*/
end:
    utref->seq_val = -(utref->seq_val);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmmtm(DBseqnum snr)

/*      G�r en storhet i aktiv modul till temporary.
 *
 *      In: snr = Storhetens sekvensnummer.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 16/3/92 J. Kjellander
 *
 *      1996-06-05 Global referens-abs(), J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Om tempor�r storhet redan finns, ta bort den.
*/
    if ( templa != DBNULL ) gmrltm();
/*
***Nu finns ingen tempor�r storhet. Nytt tempor�rt snr = tempsn.
***F�r s�kerhets skull anv�nder vi abs(snr) s� att inte en global
***referens st�ller p� h�gre niv� till problem h�r. S� var tex.
***fallet med updpp_mbs()-exrist()-gmmtm(). 1996-06-05 JK
*/
    tempsn = abs(snr);
/*
***Nya tempor�ra storhetens la f�s ur idtab p� h�gsta niv�n, DBNULL.
*/
    templa = gmrdid(DBNULL,tempsn);
/*
***Skriv dit suddad ist�llet.
*/
    wrid(tempsn,(DBptr)ERASED);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmumtm()

/*      H�mtar tillbaks temporary.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:  0 = OK.
 *          -1 = Det finns ingen temporary.
 *
 *      (C)microform ab 16/3/92 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Skriv templa p� platsen tempsn i huvudpartens idtab.
*/
    if ( templa != DBNULL )
      {
      wrid(tempsn,templa);
      templa = DBNULL;
      return(0);
      }
/*
***Det finns ingen temporary att h�mta tillbaks !
*/
    else return(-1);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmrltm()

/*      Tar bort temporary. Anropas av gmmtm() innan ny
 *      storhet gj�rs tempor�r samt av gmsvpf() f�r att
 *      inte tempor�r storhet skall lagras i PF.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:  0 = OK.
 *
 *      (C)microform ab 27/11/92 J. Kjellander
 *
 ******************************************************!*/

  {
   DBptr   orgla;
   GMRECH  hed;

/*
***Om tempor�r storhet finns, l�nka tillbaks den
***och ta bort den. L�nka d�refter in den riktiga igen.
*/
    if ( templa != DBNULL )
      {
      orgla = gmrdid(DBNULL,tempsn);
      wrid(tempsn,templa);
      rddat1((char *)&hed,templa,sizeof(GMRECH));
      DBdelete_entity(templa,hed.type);
      wrid(tempsn,orgla);
      templa = DBNULL;
      }

    return(0);
  }

/********************************************************/
