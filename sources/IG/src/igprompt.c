/*!******************************************************************/
/*  igprompt.c                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*     IGplma();         Push line in message area                  */
/*     IGptma();         Push t-string in message area              */
/*     IGwlma();         Write line in message area                 */
/*     IGwtma();         Write t-string in message area             */
/*     IGrsma();         Pop  line in message area                  */
/*    *IGqema();         Return current t-string                    */
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
#include <string.h>


#define MXMLEV 5
static char  pstack[MXMLEV+1][81];  /* Prompt stack */
       short pant = 0;              /* Stack size */

/*!****************************************************************/

        short IGplma(
        char *s,
        int   mode)

/*      Skriv ut och stacka text p� meddelanderaden.
 *
 *      In:  *s  => Pekare till str�ng.
 *          mode => IG_INP  = Prompt f�re inmatning
 *                  IG_MESS = Meddelande
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 2/1/95 J. Kjellander
 *
 ****************************************************************!*/

  {
    if ( pant < MXMLEV )
      {
      strncpy(pstack[pant],s,80);
      pstack[pant][80] = '\0';
      ++pant;
      }

    return(IGwlma(s,mode));
  }

/******************************************************************/
/*!****************************************************************/

        short IGptma(
        int tsnum,
        int mode)

/*      Skriver ut (pushar) t-str�ng p� meddelanderaden eller
 *      motsvarande.
 *
 *      In: tsnum => Numret p� t-str�ngen.
 *          mode  => IG_INP  = Prompt f�re inmatning
 *                   IG_MESS = Meddelande
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 2/1/95 J. Kjellander
 *
 ****************************************************************!*/

  {
    return(IGplma(IGgtts(tsnum),mode));
  }

/******************************************************************/
/*!****************************************************************/

        short IGwlma(
        char *s,
        int   mode)

/*      Skriv ut text p� meddelanderaden.
 *
 *      In:  *s  => Pekare till str�ng.
 *          mode => IG_INP  = Prompt f�re inmatning
 *                  IG_MESS = Meddelande
 *
 *      Ut: Inget.
 *
 *     (C)microform ab 15/11/88 J. Kjellander
 *
 ****************************************************************!*/

  {

/*
***X11/WIN32. Om mode = IG_INP kommer promten ut i inmatningsf�nstret
***utan att vi beh�ver g�ra n�got ytterligare h�r. Om mode = IG_MESS
***anv�nder vi winpac:s meddelandef�nster.
*/
#ifdef UNIX
    if ( mode == IG_MESS ) WPwlma(s);
    return(0);
#endif

#ifdef WIN32
    if ( mode == IG_MESS ) mswlma(s);
    return(0);
#endif

    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short IGwtma(short tsnum)

/*
 *      Skriver ut t-str�ng p� meddelanderaden utan att
 *      meddelandet pushas p� meddelandestacken.
 *
 *      In: tsnum => Numret p� t-str�ngen.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 2/2/95 J. Kjellander
 *
 ****************************************************************!*/

  {
    return(IGwlma(IGgtts(tsnum),IG_MESS));
  }

/******************************************************************/
/*!****************************************************************/

        short IGrsma()

/*      Suddar meddelanderaden, dvs. g�r pull p� pstack.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 ****************************************************************!*/

  {

    if ( pant > 0 ) --pant;

    if ( pant > 0 ) IGwlma(pstack[pant-1],IG_MESS);
    else            IGwlma("",IG_MESS);

    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        char *IGqema()

/*      Svarar p� fr�gan: Vilken var den sista t-str�ng som skrevs
 *      i meddelande-arean. 
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Pekare till str�ng.
 *
 *
 *     (C)microform ab 28/10-85 Ulf Johansson
 *
 *     10/10/86 pstack, J. Kjellander
 *
 ****************************************************************!*/

  {
    if ( pant > 0 )
      {
      if ( pant <= MXMLEV ) return(pstack[pant-1]);
      else return(pstack[MXMLEV-1]);
      }
    else return("");
  }

/******************************************************************/
