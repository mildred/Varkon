/*!******************************************************************/
/*  File: igID.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   IGstid();   String to ID                                       */
/*   IGidst();   ID to string                                       */
/*   IGcsid();   Compare 2 ID's                                     */
/*   IGcmid();   Compare one ID with many                           */
/*   IGcpre();   Copy ID                                            */
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
#include <string.h>
#include <ctype.h>

/*!******************************************************/

        short IGstid(
        char *idstr,
        DBId *idvek)

/*      Konvertera en str�ng av ASCII-tecken till en ID.
 *
 *      In: idstr => Pekare till char-array.
 *          idvek => Pekare till lista av id-structures.
 *
 *      Ut: *idvek => L�nkad lista av DBId   .
 *
 *      FV:        0 => Ok.
 *                -1 => Syntaxfel.
 *
 *      (C)microform ab 6/8/85 J. Kjellander
 *
 *      9/11/85  Ordningsnummer, J. Kjellander
 *      25/11/85 Bytt till punkt, J. Kjellander
 *      22/12/86 Globala ref, J. Kjellander
 *      23/10/89 386-Xenix, EOF i sscanf(), J. Kjellander
 *
 ******************************************************!*/

  {
    short  i,niv,n;
    short  snr,onr;
    char   c;
    bool   global;

/*
***Lite initiering.
*/
    i = 0;
    niv = 0;
    global = FALSE;
/*
***Lokal eller global?
*/
    while ( isspace(idstr[i]) != 0 ) ++i;  /* Skippa blanka */

    if ( strncmp(&idstr[i],"##",2) == 0 )
      {
      global = TRUE;
      ++i;
      }
/*
***Beta av resten.
*/
loop:
    while ( isspace(idstr[i]) != 0 ) ++i;  /* Skippa blanka */
    n = sscanf( &idstr[i], "%c", &c);      /* L�s "#" */
    if ( n == EOF && niv > 0 ) goto end;
/*
***I samband med �verg�ng till 386-Xenix fick denna rad l�ggas till
*/
    if ( n == 0   && niv > 0 ) goto end;
/*
***f�r att slut p� str�ngen skulle uppt�ckas. sscanf()
***returnerar inte EOF f�rr�n vid n�sta l�sning. Kan detta
***vara standard???
*/
    if ( n == EOF && niv == 0 ) return(-1);
    if ( c != '#' ) return(-1);

    ++i;
    n = sscanf( &idstr[i], "%hd", &snr);   /* L�s sekv.nr. */
    if ( n == 0 || n == EOF ) return(-1);
    if ( snr < 0 ) return(-1);
    while ( isdigit(idstr[i]) != 0 ) ++i;  /* Skippa siffror */

    n = sscanf( &idstr[i], "%c", &c);      /* L�s ev. "." */
    if ( n != EOF && c == '.' )
      {
      ++i;
      n = sscanf( &idstr[i], "%hd", &onr);  /* L�s ordn.nr. */
      if ( n == 0 || n == EOF ) return(-1);
      if ( onr < 1 ) return(-1);
      while ( isdigit(idstr[i]) != 0 ) ++i;  /* Skippa siffror */
      }
    else
      {
      onr = 1;
      }

    idvek[niv].seq_val = snr;
    idvek[niv].ord_val = onr;
    idvek[niv].p_nextre = &idvek[niv+1];
    ++niv;

    goto loop;

end:
    idvek[niv-1].p_nextre = NULL;
    if ( global ) idvek[0].seq_val = -(idvek[0].seq_val);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGidst(
        DBId *idvek,
        char *idstr)

/*      Konverterar en identitet till str�ng av ASCII-
 *      tecken.
 *
 *      In: idvek => Pekare till lista av id-structures.
 *          idstr => Pekare till char-array.
 *
 *      Ut: *idstr => Str�ng, null-terminated.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 28/1/85 J. Kjellander
 *
 *      16/10/85 Ordningsnummer, J. Kjellandser
 *      25/11/85 Bytt till punkt, J. Kjellander
 *      22/12/86 Globala ref, J. Kjellander
 *
 ******************************************************!*/

  {
    char     tmp[80];
    DBId    *idptr;

    idstr[0] = '\0';
    idptr = idvek;

    if ( idptr->seq_val < 0 ) strcat(idstr,"#");

    do
      {
      sprintf(tmp,"#%d.%d",abs(idptr->seq_val),idptr->ord_val);
      strcat(idstr,tmp);
      }
    while ( (idptr=idptr->p_nextre) != NULL );

    return(0);
  }

/********************************************************/
/*******************************************************!*/

        bool IGcsid(
        DBId *pid1,
        DBId *pid2)

/*      J�mf�r tv� enstaka identiteter.
 *
 *      In: pid1  => Pekare till storhet1:s identitet.
 *          pid2  => Pekare till storhet2:s identitet.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  => pid1 =  pid2.
 *          FALSE => pid1 != pid2.
 *
 *      (C)microform ab 22/8/85 R. Svedin.
 *
 ******************************************************!*/

  {
     DBId     *id1pek;
     DBId     *id2pek;

/*
***J�mf�r sekvensnummer och ordningsnummer.
*/
     if ( pid1->seq_val  != pid2->seq_val )  return(FALSE);
     else if ( pid1->ord_val  != pid2->ord_val )  return(FALSE);
/*
***Sekvensnr. och ordningsnr. �r lika. Kolla om b�da har vidarepekare.
*/
     if ( pid1->p_nextre != NULL && pid2->p_nextre != NULL )
       {
       id1pek = pid1->p_nextre;
       id2pek = pid2->p_nextre;
       return( IGcsid(id1pek, id2pek));
       }
/*
***En eller b�da har pekare = NULL. Kolla om b�da har det.
*/
     else if ( pid1->p_nextre == NULL && 
               pid2->p_nextre == NULL ) return(TRUE);
/*
***Bara den ena hade pekare = NULL.
*/
     else return(FALSE);

  }

/********************************************************/
/******************************************************!*/

        bool IGcmid(
        DBId *idpek,
        DBId  idvek[][MXINIV],
        short vn)

/*      J�mf�r en identitet med ett antal andra identiteter.
 *
 *      In: idpek => Pekare till storhet1:s identitet.
 *          idvek => Pekare till en vektor av identiteter.
 *          vn    => Antal element i vektorn.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  => idpek finns redan med i idvek.
 *          FALSE => idpek finns ej med i idvek.
 *
 *      (C)microform ab 23/8/85 R. Svedin.
 * 
 *      16/3/88  Omgjord f�r IGgmid(), J. Kjellander
 *
 ******************************************************!*/

  {
     short i;

/*
***J�mf�r
*/
     for ( i = 0; i < vn; ++i )
       {
       if ( IGcsid(idpek,&idvek[i][0]))
         {
         return(TRUE);       /* Storheten finns i vektorn */
         }
       }
      return(FALSE);         /* Storheten finns ej med i vektorn */
  }

/*********************************************************/
/*!******************************************************/

        short IGcpre(
        DBId *frompk,
        DBId *topk)

/*      Kopierar en referens.
 *
 *      In: frompk = Pekare till gammal referens.
 *          topk   = Pekare till ny referens (array).
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 2/6/86 J. Kjellander
 *
 ******************************************************!*/

  {
    short i;

    for ( i=0;; ++i )
      {
      (topk+i)->seq_val = (frompk+i)->seq_val;
      (topk+i)->ord_val = (frompk+i)->ord_val;
      if ( (frompk+i)->p_nextre == NULL )
        {
        (topk+i)->p_nextre = NULL;
        break;
        }
      else
        {
        (topk+i)->p_nextre = &topk[i+1];
        }
      }

    return(0);
  }
  
/********************************************************/
